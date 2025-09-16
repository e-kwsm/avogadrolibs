/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#ifndef AVOGADRO_CORE_ANGLETOOLS_H
#define AVOGADRO_CORE_ANGLETOOLS_H

#include <avogadro/core/vector.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

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
 * Calculate the Wilson out-of-plane angle for a central atom.
 * Uses the Wilson angle definition:
 *   sin(chi) = (u1 x u2) . u3 / |u1 x u2|
 * where u1, u2, u3 are unit vectors from the central atom to b, c, d.
 * @param point The central atom.
 * @param b The first surrounding atom.
 * @param c The second surrounding atom.
 * @param d The third surrounding atom (out-of-plane atom).
 * @return The out-of-plane angle in degrees.
 */
inline Real outOfPlaneAngle(const Vector3& point, const Vector3& b,
                            const Vector3& c, const Vector3& d);

/**
 * Remove the jumps a periodic coordinate makes when it crosses the end of its
 * range, by taking the shortest step between each pair of neighbours.
 * @param values The series to unwrap, in place, in the order it was measured
 * @param period The period of the coordinate, e.g. 360 for degrees
 *
 * A torsion scan that walks through +/-180 degrees otherwise reads as a jump
 * the width of the whole axis. The result can lie outside the original range;
 * shiftValuesToWindow() puts it back.
 */
template <typename T>
inline void unwrapPeriodicValues(std::vector<T>& values, T period)
{
  if (values.empty() || period <= T(0))
    return;

  const T halfPeriod = period / T(2);
  T offset = T(0);
  T previous = values[0];

  for (size_t i = 1; i < values.size(); ++i) {
    T current = values[i] + offset;
    const T delta = current - previous;
    if (delta > halfPeriod) {
      offset -= period;
      current -= period;
    } else if (delta < -halfPeriod) {
      offset += period;
      current += period;
    }

    values[i] = current;
    previous = current;
  }
}

/**
 * Slide a periodic series by whole periods into the window that holds the most
 * of its values, breaking ties towards the centre of that window.
 * @param values The series to shift, in place
 * @param period The period of the coordinate, e.g. 360 for degrees
 * @param minimum The lower edge of the preferred window, e.g. -180
 * @param maximum The upper edge of the preferred window, e.g. 180
 *
 * Unwrapping leaves a series anywhere on the real line. This puts it back
 * where it is expected to be read, without reintroducing the jumps.
 */
template <typename T>
inline void shiftValuesToWindow(std::vector<T>& values, T period, T minimum,
                                T maximum)
{
  if (values.empty() || period <= T(0) || minimum >= maximum)
    return;

  const T lowest = *std::min_element(values.begin(), values.end());
  const T highest = *std::max_element(values.begin(), values.end());
  const T minShift = std::floor((lowest - maximum) / period) * period;
  const T maxShift = std::ceil((highest - minimum) / period) * period;

  int bestCount = -1;
  T bestShift = T(0);
  T bestCenterDistance = std::numeric_limits<T>::max();
  const T preferredCenter = (minimum + maximum) / T(2);

  for (T shift = minShift; shift <= maxShift; shift += period) {
    int count = 0;
    T centerDistance = T(0);
    for (T value : values) {
      const T shifted = value - shift;
      if (shifted >= minimum && shifted <= maximum) {
        ++count;
        centerDistance += std::fabs(shifted - preferredCenter);
      }
    }

    if (count > bestCount ||
        (count == bestCount && centerDistance < bestCenterDistance)) {
      bestCount = count;
      bestShift = shift;
      bestCenterDistance = centerDistance;
    }
  }

  for (T& value : values)
    value -= bestShift;
}

} // namespace Avogadro

#endif // AVOGADRO_CORE_ANGLETOOLS_H
