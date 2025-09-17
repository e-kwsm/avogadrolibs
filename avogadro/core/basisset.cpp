/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include "basisset.h"

namespace Avogadro::Core {

unsigned int BasisSet::lumo(ElectronType type) const
{
  if (type == Beta) {
    // check if we have occupancy data
    if (m_moOccupancy[1].size() > 0) {
      for (unsigned int i = 0; i < m_moOccupancy[1].size(); ++i) {
        if (m_moOccupancy[1][i] == 0)
          return i;
      }
    }
  } else {
    // alpha or paired
    // check if we have occupancy data - more accurate
    if (m_moOccupancy[0].size() > 0) {
      for (unsigned int i = 0; i < m_moOccupancy[0].size(); ++i) {
        if (m_moOccupancy[0][i] == 0)
          return i;
      }
    }
  }
  // fall back to electron count
  return m_electrons[0] / 2 + 1;
}

void BasisSet::setElectronCount(unsigned int n, ElectronType type)
{
  switch (type) {
    case Paired:
      m_electrons[0] = n;
      m_electrons[1] = 0;
      break;
    case Alpha:
      m_electrons[0] = n;
      break;
    case Beta:
      m_electrons[1] = n;
      break;
    default:
      // Shouldn't hit this condition.
      ;
  }
}

unsigned int BasisSet::electronCount(ElectronType type) const
{
  switch (type) {
    case Paired:
    case Alpha:
      return m_electrons[0];
    case Beta:
      return m_electrons[1];
    default:
      // Shouldn't hit this condition.
      return 0;
  }
}

void BasisSet::setSymmetryLabels(const std::vector<std::string>& labels,
                                 ElectronType type)
{
  if (type == Paired || type == Alpha)
    m_symmetryLabels[0] = labels;
  else
    m_symmetryLabels[1] = labels;
}

void BasisSet::setMolecularOrbitalEnergy(const std::vector<double>& energies,
                                         ElectronType type)
{
  if (type == Beta)
    m_moEnergy[1] = energies;
  else
    m_moEnergy[0] = energies;
}

void BasisSet::setMolecularOrbitalOccupancy(
  const std::vector<unsigned char>& occ, ElectronType type)
{
  if (type == Beta)
    m_moOccupancy[1] = occ;
  else
    m_moOccupancy[0] = occ;
}

} // namespace Avogadro::Core
