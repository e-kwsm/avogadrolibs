/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#ifndef AVOGADRO_CORE_H
#define AVOGADRO_CORE_H

#include <cstddef>
#include <limits>

/** Prevent compiler error when using std::numeric_limits<T>::max() */
#if defined(_MSC_VER) && defined(max)
#undef max
#endif

/**
 * This macro marks a parameter as unused. Its purpose is to disable the
 * compiler from emitting unused parameter warnings.
 */
#define AVO_UNUSED(variable) (void)variable

/**
 * This macro marks a class as not copyable. It should be used in the private
 * section of a class's declaration.
 */
#define AVO_DISABLE_COPY(Class)                                                \
  Class(const Class&);                                                         \
  Class& operator=(const Class&);

namespace Avogadro {

/** Typedef for a real number. */
using Real = double;

/** Typedef for indices and sizes. */
using Index = size_t;
inline constexpr Index MaxIndex = std::numeric_limits<Index>::max();

/** Used to represent an invalid atomic number. */
constexpr unsigned char InvalidElement = 255;

/**
 * Minimum value for atomic numbers that represent custom, non-elemental
 *  particles. */
constexpr unsigned char CustomElementMin = 128;

/**
 * Maximum value for atomic numbers that represent custom, non-elemental
 *  particles. */
constexpr unsigned char CustomElementMax = 254;

/**
 * Count of atomic number values that are used to represent custom,
 * non-elemental particles. */
inline constexpr unsigned char CustomElementCount =
  CustomElementMax - CustomElementMin + 1;

namespace Core {
/**
 * @return True if @a atomicNumber denotes a custom element type.
 */
constexpr bool isCustomElement(unsigned char atomicNumber)
{
  return atomicNumber >= CustomElementMin && atomicNumber <= CustomElementMax;
}
} // namespace Core

/** Unit conversion factors. @{ */
template <typename T = Real>
constexpr T PI = static_cast<T>(3.141592653589793238462643);
constexpr auto PI_D = PI<double>;
constexpr auto PI_F = PI<float>;

template <typename T = Real>
constexpr T DEG_TO_RAD = static_cast<T>(PI_D / 180.0);
constexpr auto DEG_TO_RAD_D = DEG_TO_RAD<double>;
constexpr auto DEG_TO_RAD_F = DEG_TO_RAD<float>;

template <typename T = Real>
constexpr T RAD_TO_DEG = static_cast<T>(180.0 / PI_D);
constexpr auto RAD_TO_DEG_D = RAD_TO_DEG<double>;
constexpr auto RAD_TO_DEG_F = RAD_TO_DEG<float>;

// from NIST
template <typename T = Real>
constexpr Real BOHR_TO_ANGSTROM = static_cast<T>(0.529177210544);
constexpr auto BOHR_TO_ANGSTROM_D = BOHR_TO_ANGSTROM<double>;
constexpr auto BOHR_TO_ANGSTROM_F = BOHR_TO_ANGSTROM<float>;

constexpr double ANGSTROM_TO_BOHR_D = 1.0 / BOHR_TO_ANGSTROM_D;
constexpr float ANGSTROM_TO_BOHR_F = static_cast<float>(ANGSTROM_TO_BOHR_D);
constexpr Real ANGSTROM_TO_BOHR = static_cast<Real>(ANGSTROM_TO_BOHR_D);

constexpr double HARTREE_TO_EV_D = 27.211386245981;
constexpr float HARTREE_TO_EV_F = static_cast<float>(HARTREE_TO_EV_D);
constexpr Real HARTREE_TO_EV = static_cast<Real>(HARTREE_TO_EV_D);

// thermochemical calorie
template <typename T = Real>
constexpr Real KCAL_TO_KJ = static_cast<T>(4.184);
constexpr double KCAL_TO_KJ_D = KCAL_TO_KJ<double>;
constexpr float KCAL_TO_KJ_F = KCAL_TO_KJ<float>;
/** @} */

} // namespace Avogadro

#endif // AVOGADRO_CORE_H
