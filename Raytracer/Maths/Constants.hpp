///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Mathematical constants and conversion factors used throughout: game engine
// computations, physics, and graphics.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Single-precision constants.
///////////////////////////////////////////////////////////////////////////////

#define PI                              (3.14159265358979323846f)
#define SMALL_NUMBER                    (1.e-8f)
#define KINDA_SMALL_NUMBER              (1.e-4f)
#define BIG_NUMBER                      (3.4e+38f)
#define EULERS_NUMBER                   (2.71828182845904523536f)
#define GOLDEN_RATIO                    (1.6180339887498948482045868343656381f)
#define FLOAT_NON_FRACTIONAL            (8388608.f)

///////////////////////////////////////////////////////////////////////////////
// Inverses and multiples of Pi.
///////////////////////////////////////////////////////////////////////////////

#define INV_PI                          (0.31830988618f)
#define HALF_PI                         (1.57079632679f)
#define TWO_PI                          (6.28318530717f)
#define PI_SQUARED                      (9.86960440108f)

///////////////////////////////////////////////////////////////////////////////
// Square roots and their inverses.
///////////////////////////////////////////////////////////////////////////////

#define SQRT_2                          (1.4142135623730950488016887242097f)
#define SQRT_3                          (1.7320508075688772935274463415059f)
#define INV_SQRT_2                      (0.70710678118654752440084436210485f)
#define INV_SQRT_3                      (0.57735026918962576450914878050196f)
#define HALF_SQRT_2                     (0.70710678118654752440084436210485f)
#define HALF_SQRT_3                     (0.86602540378443864676372317075294f)

///////////////////////////////////////////////////////////////////////////////
// Unit conversion factors.
///////////////////////////////////////////////////////////////////////////////

#define KM_TO_M                         (1000.f)
#define M_TO_KM                         (0.001f)
#define CM_TO_M                         (0.01f)
#define M_TO_CM                         (100.f)
#define CM2_TO_M2                       (0.0001f)
#define M2_TO_CM2                       (10000.f)

///////////////////////////////////////////////////////////////////////////////
// Small deltas and precision thresholds.
///////////////////////////////////////////////////////////////////////////////

#define DELTA                           (0.00001f)

#define FLOAT_NORMAL_THRESH             (0.0001f)

///////////////////////////////////////////////////////////////////////////////
// Geometric thresholds and tolerances.
///////////////////////////////////////////////////////////////////////////////

#define THRESH_POINT_ON_PLANE           (0.10f)
#define THRESH_POINT_ON_SIDE            (0.20f)
#define THRESH_POINTS_ARE_SAME          (0.00002f)
#define THRESH_POINTS_ARE_NEAR          (0.015f)
#define THRESH_NORMALS_ARE_SAME         (0.00002f)
#define THRESH_UVS_ARE_SAME             (0.0009765625f)

#define THRESH_VECTORS_ARE_NEAR         (0.0004f)

#define THRESH_SPLIT_POLY_WITH_PLANE    (0.25f)
#define THRESH_SPLIT_POLY_PRECISELY     (0.01f)
#define THRESH_ZERO_NORM_SQUARED        (0.0001f)
#define THRESH_NORMALS_ARE_PARALLEL     (0.999845f)
#define THRESH_NORMALS_ARE_ORTHOGONAL   (0.017455f)

#define THRESH_VECTOR_NORMALIZED        (0.01f)
#define THRESH_QUAT_NORMALIZED          (0.01f)
