/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include "iotests.h"

#include <gtest/gtest.h>

#include <cmath>
#include <sstream>

#include <avogadro/core/molecule.h>
#include <avogadro/core/residue.h>

#include <avogadro/io/pdbformat.h>

using Avogadro::Core::Molecule;
using Avogadro::Io::PdbFormat;

namespace {

bool hasAtomAt(const Molecule& molecule, double x, double y, double z)
{
  constexpr double tolerance = 1e-6;
  for (Avogadro::Index i = 0; i < molecule.atomCount(); ++i) {
    const auto pos = molecule.atomPosition3d(i);
    if (std::abs(pos.x() - x) < tolerance &&
        std::abs(pos.y() - y) < tolerance &&
        std::abs(pos.z() - z) < tolerance) {
      return true;
    }
  }

  return false;
}

} // namespace

TEST(PdbTest, read)
{
  PdbFormat pdb;
  Molecule molecule;
  pdb.readFile(std::string(AVOGADRO_DATA) + "/data/pdb/1MYK.pdb", molecule);

  // how many atoms?
  EXPECT_EQ(molecule.atomCount(), 780);
  EXPECT_EQ(molecule.residueCount(), 108);
}

TEST(PdbTest, altLoc)
{
  PdbFormat pdb;
  Molecule molecule;
  pdb.readFile(std::string(AVOGADRO_DATA) + "/data/pdb/1FDT.pdb", molecule);

  EXPECT_EQ(molecule.coordinate3dCount(), 2);

  EXPECT_DOUBLE_EQ(molecule.atomPosition3d(264).x(),
                   molecule.coordinate3d(1)[264].x());
  EXPECT_DOUBLE_EQ(molecule.atomPosition3d(264).y(),
                   molecule.coordinate3d(1)[264].y());
  EXPECT_DOUBLE_EQ(molecule.atomPosition3d(264).z(),
                   molecule.coordinate3d(1)[264].z());

  EXPECT_TRUE(
    molecule.atomPosition3d(265).x() != molecule.coordinate3d(1)[265].x() ||
    molecule.atomPosition3d(265).y() != molecule.coordinate3d(1)[265].y() ||
    molecule.atomPosition3d(265).z() != molecule.coordinate3d(1)[265].z());
}

TEST(PdbTest, readVillin)
{
  PdbFormat pdb;
  Molecule molecule;
  pdb.readFile(std::string(AVOGADRO_DATA) + "/data/pdb/villin_N68H.pdb",
               molecule);

  // how many atoms?
  EXPECT_EQ(molecule.atomCount(), 8867);

  // The reader walks the residue records of every MODEL, but atoms are only
  // added for MODEL 1 (subsequent models populate coordinate sets).  This
  // file has 10 models with 2798 residue transitions each, giving 27980.
  EXPECT_EQ(molecule.residueCount(), 27980);

  EXPECT_EQ(molecule.coordinate3dCount(), 10);
}

TEST(PdbTest, cryst1)
{
  PdbFormat pdb;
  Molecule molecule;
  pdb.readFile(std::string(AVOGADRO_DATA) + "/data/pdb/cryst1.pdb", molecule);

  EXPECT_EQ(molecule.atomCount(), 7);

  // make sure it has a unit cell
  bool hasUnitCell = (molecule.unitCell() != nullptr);
  EXPECT_TRUE(hasUnitCell);
}

TEST(PdbTest, cryst2)
{
  // Crash fixed with #2182.  The ATOM records in this file are missing the
  // residue name / chain / residue-id columns, so the hardened reader now
  // rejects them.  The important guarantee is no crash and that CRYST1
  // (parsed before any ATOM record) still produces a unit cell.
  PdbFormat pdb;
  Molecule molecule;
  const bool readOk =
    pdb.readFile(std::string(AVOGADRO_DATA) + "/data/pdb/cryst2.pdb", molecule);

  EXPECT_FALSE(readOk);
  EXPECT_EQ(molecule.atomCount(), 0);

  // make sure it has a unit cell
  bool hasUnitCell = (molecule.unitCell() != nullptr);
  EXPECT_TRUE(hasUnitCell);
}

TEST(PdbTest, cryst3)
{
  // Crash fixed with #2182
  PdbFormat pdb;
  Molecule molecule;
  pdb.readFile(std::string(AVOGADRO_DATA) + "/data/pdb/cryst3.pdb", molecule);

  EXPECT_EQ(molecule.atomCount(), 7);

  // make sure it has a unit cell
  bool hasUnitCell = (molecule.unitCell() != nullptr);
  EXPECT_TRUE(hasUnitCell);
}

TEST(PdbTest, pdb1)
{
  // Windows crash reported as #2456
  PdbFormat pdb;
  Molecule molecule;
  pdb.readFile(std::string(AVOGADRO_DATA) + "/data/pdb/1.pdb", molecule);

  // should be 60 atoms, sequence is TRP-LEU-ASN
  EXPECT_EQ(molecule.atomCount(), 60);

  // should be three residues
  EXPECT_EQ(molecule.residueCount(), 3);

  auto residues = molecule.residues();
  EXPECT_EQ(residues[0].residueName(), "TRP");
  EXPECT_EQ(residues[1].residueName(), "LEU");
  EXPECT_EQ(residues[2].residueName(), "ASN");
}

TEST(PdbTest, biomtPreservesCoordinateSets)
{
  const std::string pdbData =
    "REMARK 350 BIOMOLECULE: 1\n"
    "REMARK 350 APPLY THE FOLLOWING TO CHAINS: A\n"
    "REMARK 350   BIOMT1   1  1.000000  0.000000  0.000000        0.00000\n"
    "REMARK 350   BIOMT2   1  0.000000  1.000000  0.000000        0.00000\n"
    "REMARK 350   BIOMT3   1  0.000000  0.000000  1.000000        0.00000\n"
    "REMARK 350   BIOMT1   2  1.000000  0.000000  0.000000       10.00000\n"
    "REMARK 350   BIOMT2   2  0.000000  1.000000  0.000000        0.00000\n"
    "REMARK 350   BIOMT3   2  0.000000  0.000000  1.000000        0.00000\n"
    "MODEL        1\n"
    "ATOM      1  N   GLY A   1       1.000   2.000   3.000  1.00 20.00        "
    "   N  \n"
    "TER       2      GLY A   1\n"
    "ENDMDL\n"
    "MODEL        2\n"
    "ATOM      1  N   GLY A   1       4.000   5.000   6.000  1.00 20.00        "
    "   N  \n"
    "TER       2      GLY A   1\n"
    "ENDMDL\n"
    "END\n";

  PdbFormat pdb;
  Molecule molecule;
  std::istringstream input(pdbData);

  EXPECT_TRUE(pdb.read(input, molecule));
  EXPECT_EQ(molecule.atomCount(), 2);
  EXPECT_EQ(molecule.coordinate3dCount(), 2);
  EXPECT_EQ(molecule.coordinate3d(0).size(), molecule.atomCount());
  EXPECT_EQ(molecule.coordinate3d(1).size(), molecule.atomCount());

  EXPECT_DOUBLE_EQ(molecule.atomPosition3d(1).x(), 11.0);
  EXPECT_DOUBLE_EQ(molecule.atomPosition3d(1).y(), 2.0);
  EXPECT_DOUBLE_EQ(molecule.atomPosition3d(1).z(), 3.0);
  EXPECT_DOUBLE_EQ(molecule.coordinate3d(0)[1].x(), 11.0);
  EXPECT_DOUBLE_EQ(molecule.coordinate3d(1)[1].x(), 14.0);
  EXPECT_DOUBLE_EQ(molecule.coordinate3d(1)[1].y(), 5.0);
  EXPECT_DOUBLE_EQ(molecule.coordinate3d(1)[1].z(), 6.0);
}

TEST(PdbTest, biomtKeepsChainGroupsSeparated)
{
  const std::string pdbData =
    "REMARK 350 BIOMOLECULE: 1\n"
    "REMARK 350 APPLY THE FOLLOWING TO CHAINS: A\n"
    "REMARK 350   BIOMT1   1  1.000000  0.000000  0.000000        0.00000\n"
    "REMARK 350   BIOMT2   1  0.000000  1.000000  0.000000        0.00000\n"
    "REMARK 350   BIOMT3   1  0.000000  0.000000  1.000000        0.00000\n"
    "REMARK 350   BIOMT1   2  1.000000  0.000000  0.000000       10.00000\n"
    "REMARK 350   BIOMT2   2  0.000000  1.000000  0.000000        0.00000\n"
    "REMARK 350   BIOMT3   2  0.000000  0.000000  1.000000        0.00000\n"
    "REMARK 350 APPLY THE FOLLOWING TO CHAINS: B\n"
    "REMARK 350   BIOMT1   1  1.000000  0.000000  0.000000        0.00000\n"
    "REMARK 350   BIOMT2   1  0.000000  1.000000  0.000000        0.00000\n"
    "REMARK 350   BIOMT3   1  0.000000  0.000000  1.000000        0.00000\n"
    "REMARK 350   BIOMT1   2  1.000000  0.000000  0.000000        0.00000\n"
    "REMARK 350   BIOMT2   2  0.000000  1.000000  0.000000       20.00000\n"
    "REMARK 350   BIOMT3   2  0.000000  0.000000  1.000000        0.00000\n"
    "ATOM      1  N   GLY A   1       1.000   1.000   1.000  1.00 20.00        "
    "   N  \n"
    "ATOM      2  N   GLY B   2       2.000   2.000   2.000  1.00 20.00        "
    "   N  \n"
    "TER       3      GLY B   2\n"
    "END\n";

  PdbFormat pdb;
  Molecule molecule;
  std::istringstream input(pdbData);

  EXPECT_TRUE(pdb.read(input, molecule));
  EXPECT_EQ(molecule.atomCount(), 4);
  EXPECT_TRUE(hasAtomAt(molecule, 1.0, 1.0, 1.0));
  EXPECT_TRUE(hasAtomAt(molecule, 2.0, 2.0, 2.0));
  EXPECT_TRUE(hasAtomAt(molecule, 11.0, 1.0, 1.0));
  EXPECT_TRUE(hasAtomAt(molecule, 2.0, 22.0, 2.0));
  EXPECT_FALSE(hasAtomAt(molecule, 12.0, 2.0, 2.0));
  EXPECT_FALSE(hasAtomAt(molecule, 1.0, 21.0, 1.0));
}

TEST(PdbTest, chainChangeStartsNewResidue)
{
  const std::string pdbText = "ATOM      1  CA  GLY A   1      11.000  12.000  "
                              "13.000  1.00 20.00           C\n"
                              "TER       2      GLY A   1\n"
                              "ATOM      3  CA  GLY B   1      21.000  22.000  "
                              "23.000  1.00 20.00           C\n"
                              "END\n";

  PdbFormat pdb;
  Molecule molecule;
  ASSERT_TRUE(pdb.readString(pdbText, molecule)) << pdb.error();

  ASSERT_EQ(molecule.residueCount(), 2);
  EXPECT_EQ(molecule.residue(0).chainId(), 'A');
  EXPECT_EQ(molecule.residue(0).residueId(), 1);
  EXPECT_EQ(molecule.residue(1).chainId(), 'B');
  EXPECT_EQ(molecule.residue(1).residueId(), 1);
}
