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
 * @file BrineEnthalpyFunction.cpp
 */

#include "constitutive/fluid/PVTFunctions/BrineEnthalpyFunction.hpp"

namespace geosx
{

using namespace stringutilities;

namespace PVTProps
{

void BrineEnthalpyFunction::Evaluation( EvalVarArgs const & pressure, EvalVarArgs const & temperature,
                                        arraySlice1d< EvalVarArgs const > const & phaseComposition, EvalVarArgs & value, bool useMass ) const
{
  GEOSX_UNUSED_VAR(phaseComposition);
  GEOSX_UNUSED_VAR(useMass);

  localIndex const numComponents = phaseComposition.size();

  value.m_var =  0.001 * pressure.m_var + 1.0 * temperature.m_var;
  // pressure derivative
  value.m_der[0] = 0.001;
  // temperature derivative
  value.m_der[numComponents+1] = 1.0;
}



REGISTER_CATALOG_ENTRY( PVTFunction,
                        BrineEnthalpyFunction,
                        string_array const &, string_array const &, real64_array const & )

} // namespace PVTProps
} // namespace geosx