/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#ifndef AVOGADRO_CORE_MDLVALENCE_P_H
#define AVOGADRO_CORE_MDLVALENCE_P_H

namespace Avogadro::Core {

/**
 * Calculate the full valency (e.g. number of expected bonds) for a given atom.
 * This function is adapted from the MDL valence model to indicate when an atom
 * is overbonded.
 * @param atomicNumber Atomic number of atom.
 * @param charge Formal charge of atom.
 * @param numBonds Number of existing bonds to atom.
 * @return The total number of expected bonds to the atom to satisfy valency.
 * May be less than @a numBonds if atom is overbonded.
 */
unsigned int atomValence(const unsigned char atomicNumber, const int charge,
                         const unsigned int numBonds);

} // end namespace Avogadro::Core

#endif // AVOGADRO_CORE_MDLVALENCE_P_H
