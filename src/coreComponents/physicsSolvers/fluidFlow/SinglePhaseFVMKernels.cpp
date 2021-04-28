/*
 * ------------------------------------------------------------------------------------------------------------
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * Copyright (c) 2018-2020 Lawrence Livermore National Security LLC
 * Copyright (c) 2018-2020 The Board of Trustees of the Leland Stanford Junior University
 * Copyright (c) 2018-2020 Total, S.A
 * Copyright (c) 2019-     GEOSX Contributors
 * All rights reserved
 *
 * See top level LICENSE, COPYRIGHT, CONTRIBUTORS, NOTICE, and ACKNOWLEDGEMENTS files for details.
 * ------------------------------------------------------------------------------------------------------------
 */

/**
 * @file SinglePhaseFVMKernels.cpp
 */

#include "SinglePhaseFVMKernels.hpp"

namespace geosx
{

namespace SinglePhaseFVMKernels
{
GEOSX_HOST_DEVICE
void
FluxKernel::compute( localIndex const numFluxElems,
                     arraySlice1d< localIndex const > const & seri,
                     arraySlice1d< localIndex const > const & sesri,
                     arraySlice1d< localIndex const > const & sei,
                     real64 const (&transmissibility)[2],
                     real64 const (&dTrans_dPres)[2],
                     ElementViewConst< arrayView1d< real64 const > > const & pres,
                     ElementViewConst< arrayView1d< real64 const > > const & dPres,
                     ElementViewConst< arrayView1d< real64 const > > const & gravCoef,
                     ElementViewConst< arrayView2d< real64 const > > const & dens,
                     ElementViewConst< arrayView2d< real64 const > > const & dDens_dPres,
                     ElementViewConst< arrayView1d< real64 const > > const & mob,
                     ElementViewConst< arrayView1d< real64 const > > const & dMob_dPres,
                     real64 const dt,
                     arraySlice1d< real64 > const & flux,
                     arraySlice2d< real64 > const & fluxJacobian )
{

  GEOSX_UNUSED_VAR( numFluxElems );
  localIndex constexpr maxStencil = CellElementStencilTPFA::MAX_STENCIL_SIZE;
  stackArray1d< real64, maxStencil > dDensMean_dP( 2 );
  stackArray1d< real64, maxStencil > dFlux_dP( 2 );

  // average density
  real64 densMean = 0.0;
  for( localIndex ke = 0; ke < 2; ++ke )
  {
    densMean        += 0.5 * dens[seri[ke]][sesri[ke]][sei[ke]][0];
    dDensMean_dP[ke] = 0.5 * dDens_dPres[seri[ke]][sesri[ke]][sei[ke]][0];
  }

  // compute potential difference
  real64 potDif = 0.0;
  real64 sumWeightGrav = 0.0;
  real64 potScale = 0.0;

  for( localIndex ke = 0; ke < 2; ++ke )
  {
    localIndex const er  = seri[ke];
    localIndex const esr = sesri[ke];
    localIndex const ei  = sei[ke];

    real64 const pressure = pres[er][esr][ei] + dPres[er][esr][ei];
    real64 const gravD = gravCoef[er][esr][ei];
    real64 const pot = transmissibility[ke] * ( pressure - densMean * gravD );

    potDif += pot;
    sumWeightGrav += gravD;
    potScale = fmax( potScale, fabs( pot ) );
  }

  // compute upwinding tolerance
  real64 constexpr upwRelTol = 1e-8;
  real64 const upwAbsTol = fmax( potScale * upwRelTol, LvArray::NumericLimits< real64 >::epsilon );

  // decide mobility coefficients - smooth variation in [-upwAbsTol; upwAbsTol]
  real64 const alpha = ( potDif + upwAbsTol ) / ( 2 * upwAbsTol );

  real64 mobility{};
  real64 dMobility_dP[2]{};
  if( alpha <= 0.0 || alpha >= 1.0 )
  {
    // happy path: single upwind direction
    localIndex const ke = 1 - localIndex( fmax( fmin( alpha, 1.0 ), 0.0 ) );
    mobility = mob[seri[ke]][sesri[ke]][sei[ke]];
    dMobility_dP[ke] = dMob_dPres[seri[ke]][sesri[ke]][sei[ke]];
  }
  else
  {
    // sad path: weighted averaging
    real64 const mobWeights[2] = { alpha, 1.0 - alpha };
    for( localIndex ke = 0; ke < 2; ++ke )
    {
      mobility += mobWeights[ke] * mob[seri[ke]][sesri[ke]][sei[ke]];
      dMobility_dP[ke] = mobWeights[ke] * dMob_dPres[seri[ke]][sesri[ke]][sei[ke]];
    }
  }

  // compute the final flux and derivatives
  real64 const fluxVal = mobility * potDif;
  for( localIndex ke = 0; ke < 2; ++ke )
  {
    real64 const dFlux_dTrans = mobility  * (  1 - dDensMean_dP[ke] * sumWeightGrav ) + dMobility_dP[ke] * potDif;

    dFlux_dP[ke] = mobility * transmissibility[ke] * (  1 - dDensMean_dP[ke] * sumWeightGrav ) +
                   dFlux_dTrans * dTrans_dPres[ke];
  }

  // populate local flux vector and derivatives
  flux[0] =  dt * fluxVal;
  flux[1] = -dt * fluxVal;

  for( localIndex ke = 0; ke < 2; ++ke )
  {
    fluxJacobian[0][ke] =  dt * dFlux_dP[ke];
    fluxJacobian[1][ke] = -dt * dFlux_dP[ke];
  }
}

}// namespace SinglePhaseFVMKernels

} // namespace geosx
