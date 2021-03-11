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
 * @file FaceElementStencil.hpp
 */

#ifndef GEOSX_FINITEVOLUME_FACEELEMENTSTENCIL_HPP_
#define GEOSX_FINITEVOLUME_FACEELEMENTSTENCIL_HPP_

#include "StencilBase.hpp"

namespace geosx
{

/// @cond DO_NOT_DOCUMENT
// TODO remove! This option allows for the creation of new mass inside a newly
// created FaceElement. The new mass will be equal to:
// creationMass = defaultDensity * defaultAperture * faceArea.
// If 0, then the beginning of step density is artificially set to zero...which
// may cause some newton convergence problems.
#define ALLOW_CREATION_MASS 1


// TODO remove! This option sets the pressure in a newly created FaceElement to
// be the lowest value of all attached non-new FaceElements.
#define SET_CREATION_PRESSURE 1

// TODO remove! This option sets the nodal displacements attached a newly
// created FaceElement to some scalar fraction of the aperture of the
// lowest attached non-new FaceElements.
#define SET_CREATION_DISPLACEMENT 0
/// @endcond

/**
 * @struct FaceElementStencil_Traits
 * Struct to predeclare the types and constexpr values of FaceElementStencil so that they may be used in
 * StencilBase.
 */
struct FaceElementStencil_Traits
{
  /// The array type that will be used to store the indices of the stencil contributors
  using IndexContainerType = ArrayOfArrays< localIndex >;

  /// The array view type for the stencil indices
  using IndexContainerViewType = ArrayOfArraysView< localIndex >;

  /// The array view to const type for the stencil indices
  using IndexContainerViewConstType = ArrayOfArraysView< localIndex const >;

  /// The array type that is used to store the weights of the stencil contributors
  using WeightContainerType = ArrayOfArrays< real64 >;

  /// The array view type for the stencil weights
  using WeightContainerViewType = ArrayOfArraysView< real64 >;

  /// The array view to const type for the stencil weights
  using WeightContainerViewConstType = ArrayOfArraysView< real64 const >;

  /// Number of points the flux is between (normally 2)
  static localIndex constexpr NUM_POINT_IN_FLUX = 6;

  /// Maximum number of points in a stencil
  static localIndex constexpr MAX_STENCIL_SIZE = 6;
};


class FaceElementStencilWrapper : public StencilWrapperBase< FaceElementStencil_Traits >,
  public FaceElementStencil_Traits
{
public:
  template< typename VIEWTYPE >
  using ElementViewConst = ElementRegionManager::ElementViewConst< VIEWTYPE >;
  template< typename VIEWTYPE >
  using CoefficientAccessor = ElementRegionManager::MaterialViewAccessor< VIEWTYPE >;

  FaceElementStencilWrapper( IndexContainerType & elementRegionIndices,
                             IndexContainerType & elementSubRegionIndices,
                             IndexContainerType & elementIndices,
                             WeightContainerType & weights )

    : StencilWrapperBase( elementRegionIndices, elementSubRegionIndices, elementIndices, weights )
  {}

  /// Default copy constructor
  FaceElementStencilWrapper( FaceElementStencilWrapper const & ) = default;

  /// Default move constructor
  FaceElementStencilWrapper( FaceElementStencilWrapper && ) = default;

  /// Deleted copy assignment operator
  FaceElementStencilWrapper & operator=( FaceElementStencilWrapper const & ) = delete;

  /// Deleted move assignment operator
  FaceElementStencilWrapper & operator=( FaceElementStencilWrapper && ) = delete;

  /**
   * @brief Give the number of stencil entries.
   * @return The number of stencil entries
   */
  virtual localIndex size() const override final
  { return m_elementRegionIndices.size(); }

  template< typename PERMTYPE >
  void computeTransmissibility( localIndex iconn,
                                PERMTYPE permeability,
                                real64 ( &transmissibility )[2] ) const;

  template< typename PERMTYPE >
  void dTrans_dPressure( localIndex iconn,
                         PERMTYPE dPerm_dPressure,
                         real64 ( &dTrans_dPressure )[2] ) const;

private:

};

/**
 * @class FaceElementStencil
 *
 * Provides management of the interior stencil points for a face elements when using Two-Point flux approximation.
 */
class FaceElementStencil : public StencilBase< FaceElementStencil_Traits, FaceElementStencil >,
  public FaceElementStencil_Traits
{
public:

  /**
   * @brief Default constructor.
   */
  FaceElementStencil();

  virtual void move( LvArray::MemorySpace const space ) override final;

  virtual void add( localIndex const numPts,
                    localIndex const * const elementRegionIndices,
                    localIndex const * const elementSubRegionIndices,
                    localIndex const * const elementIndices,
                    real64 const * const weights,
                    localIndex const connectorIndex ) override final;

  /**
   * @brief Add an entry to the stencil.
   * @param[in] numPts The number of points in the stencil entry
   * @param[in] cellCenterToEdgeCenter vectors pointing from the cell center to the edge center
   * @param[in] connectorIndex The index of the connector element that the stencil acts across
   */
  void add( localIndex const numPts,
            R1Tensor const * const cellCenterToEdgeCenter,
            localIndex const connectorIndex );


  /// Type of kernel wrapper for in-kernel update
  using StencilWrapper = FaceElementStencilWrapper;

  /**
   * @brief Create an update kernel wrapper.
   * @return the wrapper
   */
  StencilWrapper createStencilWrapper()
  {
    return StencilWrapper( m_elementRegionIndices,
                           m_elementSubRegionIndices,
                           m_elementIndices,
                           m_weights );
  }


  /**
   * @brief Return the stencil size.
   * @return the stencil size
   */
  virtual localIndex size() const override final
  { return m_elementRegionIndices.size(); }

  /**
   * @brief Give the number of stencil entries for the provided index.
   * @param[in] index the index of which the stencil size is request
   * @return The number of stencil entries for the provided index
   */
  localIndex stencilSize( localIndex index ) const
  { return m_elementRegionIndices.sizeOfArray( index ); }

  /**
   * @brief Give the array of vectors pointing from the cell center to the edge center.
   * @return The array of vectors pointing from the cell center to the edge center
   */
  ArrayOfArraysView< R1Tensor const > getCellCenterToEdgeCenters() const
  { return m_cellCenterToEdgeCenters.toViewConst(); }

private:

  ArrayOfArrays< R1Tensor > m_cellCenterToEdgeCenters;

};

template< typename PERMTYPE >
void FaceElementStencilWrapper::computeTransmissibility( localIndex iconn,
                                                         PERMTYPE permeability,
                                                         real64 (& transmissibility)[2] ) const
{
  localIndex const er0  =  m_elementRegionIndices[iconn][0];
  localIndex const esr0 =  m_elementSubRegionIndices[iconn][0];
  localIndex const ei0  =  m_elementIndices[iconn][0];

  localIndex const er1  =  m_elementRegionIndices[iconn][1];
  localIndex const esr1 =  m_elementSubRegionIndices[iconn][1];
  localIndex const ei1  =  m_elementIndices[iconn][1];

  real64 const t0 = m_weights[iconn][0] * permeability[er0][esr0][ei0][0][0]; // this is a bit insane to access perm
  real64 const t1 = m_weights[iconn][1] * permeability[er1][esr1][ei1][0][0];

  real64 const harmonicWeight   = t0*t1 / (t0+t1);
  real64 const arithmeticWeight = (t0+t1)/2;

  real64 const meanPermCoeff = 1.0; //TODO make it a member

  transmissibility[0] = meanPermCoeff * harmonicWeight + (1 - meanPermCoeff) * arithmeticWeight;
  transmissibility[1] = meanPermCoeff * harmonicWeight + (1 - meanPermCoeff) * arithmeticWeight;
}

template< typename PERMTYPE >
void FaceElementStencilWrapper::dTrans_dPressure( localIndex iconn,
                                                  PERMTYPE dPerm_dPressure,
                                                  real64 (& dTrans_dPressure )[2] ) const
{
  localIndex const er0  =  m_elementRegionIndices[iconn][0];
  localIndex const esr0 =  m_elementSubRegionIndices[iconn][0];
  localIndex const ei0  =  m_elementIndices[iconn][0];

  localIndex const er1  =  m_elementRegionIndices[iconn][1];
  localIndex const esr1 =  m_elementSubRegionIndices[iconn][1];
  localIndex const ei1  =  m_elementIndices[iconn][1];

  real64 const dt0 = m_weights[iconn][0] * dPerm_dPressure[er0][esr0][ei0][0][0];
  real64 const dt1 = m_weights[iconn][1] * dPerm_dPressure[er1][esr1][ei1][0][0];

  // TODO fix this with proper derivative calculation.
  dTrans_dPressure[0] = dt0;
  dTrans_dPressure[1] = dt1;
}

} /* namespace geosx */

#endif /* GEOSX_FINITEVOLUME_FACEELEMENTSTENCIL_HPP_ */
