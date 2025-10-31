/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include "iotests.h"

#include <gtest/gtest.h>

#include <avogadro/core/atom.h>
#include <avogadro/core/molecule.h>
#include <avogadro/core/unitcell.h>
#include <avogadro/core/vector.h>
#include <avogadro/io/turbomoleformat.h>

#include <string>

using Avogadro::BOHR_TO_ANGSTROM;
using Avogadro::Vector3;
using Avogadro::Core::Molecule;
using Avogadro::Io::TurbomoleFormat;
using namespace std::string_literals;

TEST(TurbomoleTest, readNonPeriodic)
{
  TurbomoleFormat tmol;
  Molecule molecule;
  const auto str = R"(# $cell is commented out
# $cell
# 8.0 8.0 8.0 90.0 90.0 90.0
# $end
$coord
-1.000  2.000  4.000  h
 1.568  2.000  4.000  cl f # Fixed
$end
)"s;
  EXPECT_TRUE(tmol.readString(str, molecule)) << str << '\n' << tmol.error();
  const auto* const uc = molecule.unitCell();
  EXPECT_EQ(uc, nullptr) << str << uc->cellMatrix();
  ASSERT_EQ(molecule.atomCount(), 2u) << str;
  {
    const auto& atom = molecule.atom(0);
    EXPECT_EQ(atom.atomicNumber(), 1u);
    EXPECT_EQ(atom.position3d().x() / BOHR_TO_ANGSTROM, -1.0);
    EXPECT_EQ(atom.position3d().y() / BOHR_TO_ANGSTROM, 2.0);
    EXPECT_EQ(atom.position3d().z() / BOHR_TO_ANGSTROM, 4.0);
  }
  {
    const auto& atom = molecule.atom(1);
    EXPECT_EQ(atom.atomicNumber(), 17u);
    EXPECT_EQ(atom.position3d().x() / BOHR_TO_ANGSTROM, 1.568);
    EXPECT_EQ(atom.position3d().y() / BOHR_TO_ANGSTROM, 2.0);
    EXPECT_EQ(atom.position3d().z() / BOHR_TO_ANGSTROM, 4.0);
  }
}

TEST(TurbomoleTest, readCellParameters)
{
  const auto periodic = "$periodic 3\n"s;

  auto cell = [](const std::string& extra = ""s) {
    return "$cell"s + extra + R"(
 4.0  6.0  8.0   90.0   90.0   90.0
)"s;
  };

  auto lattice = [](const std::string& extra = ""s) {
    return "$lattice"s + extra + R"(
 4.0  0.0  0.0
 0.0  6.0  0.0
 0.0  0.0  8.0
)"s;
  };

  constexpr double EPS = 1.0e-15;
  for (const auto& len : {
         ""s,      // bohr
         " angs"s, // ångström
         "angs"s   // INVALID: space required
       }) {
    for (const auto& str : {
           periodic + cell(len) + "$end"s,
           periodic + lattice(len) + "$end"s,
         }) {
      TurbomoleFormat tmol;
      Molecule molecule;
      const bool ok = tmol.readString(str, molecule);
      const auto* const uc = molecule.unitCell();
      if (len == "angs"s) {
        EXPECT_EQ(uc, nullptr) << str;
      } else {
        EXPECT_TRUE(ok) << str << '\n' << tmol.error();
        ASSERT_NE(uc, nullptr) << str;
        const double factor = len.empty() ? BOHR_TO_ANGSTROM : 1.0;
        const auto& a = uc->aVector();
        const auto& b = uc->bVector();
        const auto& c = uc->cVector();
        EXPECT_EQ(a[0] / factor, 4.0);
        EXPECT_EQ(b[1] / factor, 6.0);
        EXPECT_EQ(c[2] / factor, 8.0);
        EXPECT_EQ(a[1], 0.0);
        EXPECT_EQ(a[2], 0.0);
        // evaluates to nonzero in case of $cell
        EXPECT_NEAR(b[0], 0.0, EPS);
        EXPECT_NEAR(b[2], 0.0, EPS);
        EXPECT_NEAR(c[0], 0.0, EPS);
        EXPECT_NEAR(c[1], 0.0, EPS);
      }
    }
  }

  for (const auto& str : {
         periodic + cell() + cell(" angs"s) + "$end"s,
         periodic + cell(" angs"s) + lattice() + "$end"s,
         periodic + lattice() + cell(" angs"s) + "$end"s,
         periodic + lattice(" angs"s) + lattice() + "$end"s,
       }) {
    TurbomoleFormat tmol;
    Molecule molecule;
    EXPECT_FALSE(tmol.readString(str, molecule)) << str;
  }
}

TEST(TurbomoleTest, readOk)
{
  for (const auto& s : {
         "$periodic 1\n$cell\n6.0\n$end"s,
         "$periodic 2\n$cell\n6.0 8.0 90.0\n$end"s,
         "$periodic 3\n$cell\n6.0 8.0 10.0 90.0 89.0 78.0\n$end"s,

         R"($periodic 0
$cell
  6.0 8.0 10.0 90.0 89.0 78.0
$end
)"s,

       }) {
    TurbomoleFormat tmol;
    Molecule molecule;
    EXPECT_TRUE(tmol.readString(s, molecule)) << s << '\n' << tmol.error();
  }
}

TEST(TurbomoleTest, readErr)
{
  for (const auto& s : {
         // $periodic is invalid
         "$periodic\n$end"s,
         "$periodic F\n$end"s,
         "$periodic -1\n$end"s,
         "$periodic 4\n$end"s,
         "$periodic 1 2\n$end"s,
       }) {
    TurbomoleFormat tmol;
    Molecule molecule;
    EXPECT_FALSE(tmol.readString(s, molecule)) << s;
  }
}

TEST(TurbomoleTest, readPeriodic)
{
  for (const auto& s : {
         "$periodic 1\n$cell\n6.0\n$end"s,
         "$periodic 2\n$cell\n6.0 8.0 90.0\n$end"s,
         "$periodic 3\n$cell\n6.0 8.0 10.0 90.0 89.0 78.0\n$end"s,
       }) {
    TurbomoleFormat tmol;
    Molecule molecule;
    EXPECT_TRUE(tmol.readString(s, molecule)) << s << '\n' << tmol.error();
  }
}

TEST(TurbomoleTest, readPeriodicErr)
{
  for (const auto& s : {
         // $periodic is invalid
         "$periodic\n$end"s,
         "$periodic F\n$end"s,
         "$periodic -1\n$end"s,
         "$periodic 4\n$end"s,
         "$periodic 1 2\n$end"s,

         // $cell/$lattice is missed
         "$periodic 1\n$end"s,
         "$periodic 2\n$end"s,
         "$periodic 3\n$end"s,

         "$periodic 1\n$cellang\n1.0"s
         "$periodic 1\n$latticeang\n1.0"s
       }) {
    TurbomoleFormat tmol;
    Molecule molecule;
    EXPECT_FALSE(tmol.readString(s, molecule)) << s;
  }

  const std::map<unsigned, std::string> cells = {
    { 1, "6.0"s },
    { 2, "6.0 8.0 90.0"s },
    { 3, "6.0 8.0 10.0 90.0 90.0 90.0"s },
  };

  const std::map<unsigned, std::string> lattices = {
    { 1, "6.0"s },
    { 2, "6.0 0.0\n0.0 8.0"s },
    { 3, "6.0 0.0 0.0\n0.0 8.0 0.0\n0.0 0.0 10.0"s },
  };

  for (unsigned periodic = 1u; periodic <= 3u; periodic++) {
    for (unsigned j = 1u; j <= 3u; j++) {
      TurbomoleFormat tmol;
      Molecule molecule;
      auto s = "$periodic "s + std::to_string(periodic) + "\n$cell\n"s +
               cells.at(j) + "\n$end"s;
      if (periodic == j) {
        EXPECT_TRUE(tmol.readString(s, molecule)) << s << '\n' << tmol.error();
      } else {
        EXPECT_FALSE(tmol.readString(s, molecule)) << s;
      }
    }
  }
}
