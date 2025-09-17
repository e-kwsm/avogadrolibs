/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#ifndef AVOGADRO_CORE_ANGLETOOLS_H
#define AVOGADRO_CORE_ANGLETOOLS_H

#include "avogadrocoreexport.h"
#include <avogadro/core/vector.h>

#include <cmath>

namespace Avogadro {

/**
 * Calculate the bond angle between two bond vectors.
 * @param b0 The first bond vector (a-b)
 * @param b1 The second bond vector (b-c)
 * @return The bond angle in degrees.
 */
AVOGADROCORE_EXPORT Real bondAngle(const Vector3& b0, const Vector3& b1);

/**
 * Calculate the dihedral angle between three bond vectors.
 * @param b0 The first bond vector (a-b)
 * @param b1 The second bond vector (b-c)
 * @param b2 The third bond vector (c-d)
 * @return The dihedral angle in degrees.
 */
AVOGADROCORE_EXPORT Real dihedralAngle(const Vector3& b0, const Vector3& b1,
                                       const Vector3& b2);

/**
 * Calculate the angle between three points in space.
 * @param v1 The first point.
 * @param v2 The second point (vertex).
 * @param v3 The third point.
 * @return The angle in degrees.
 */
AVOGADROCORE_EXPORT Real calculateAngle(const Vector3& v1, const Vector3& v2,
                                        const Vector3& v3);

/**
 * Calculate the dihedral angle between four points in space.
 * @param v1 The first point.
 * @param v2 The second point.
 * @param v3 The third point.
 * @param v4 The fourth point.
 * @return The dihedral angle in degrees.
 */
AVOGADROCORE_EXPORT Real calculateDihedral(const Vector3& v1, const Vector3& v2,
                                           const Vector3& v3,
                                           const Vector3& v4);

/**
 * Calculate the out-of-plane angle for a point in space.
 * @param point The point to calculate the angle for.
 * @param b The first point in the plane.
 * @param c The second point in the plane.
 * @param d The third point in the plane.
 * @return The out-of-plane angle in degrees.
 */
AVOGADROCORE_EXPORT Real outOfPlaneAngle(const Vector3& point, const Vector3& b,
                                         const Vector3& c, const Vector3& d);

} // namespace Avogadro

#endif // AVOGADRO_CORE_ANGLETOOLS_H
