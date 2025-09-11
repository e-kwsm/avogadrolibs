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

#include <map>
#include <string>

using Avogadro::Core::Molecule;
using Avogadro::Io::TurbomoleFormat;
using namespace std::string_literals;

TEST(TurbomoleTest, readNonPeriodic)
{
  for (const auto& s : { "$coord\n0.0 0.0 0.0 pt # $comment\n4.0 0.0 0.0 h\n$end"s }) {
    TurbomoleFormat tmol;
    Molecule molecule;
    EXPECT_TRUE(tmol.readString(s, molecule)) << s << '\n' << tmol.error();
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
      auto s = "$periodic "s + std::to_string(periodic) + "\n$cell"s +
               cells.at(j) + "\n$end"s;
      if (periodic == j) {
        EXPECT_TRUE(tmol.readString(s, molecule)) << s << '\n' << tmol.error();
      } else {
        EXPECT_FALSE(tmol.readString(s, molecule)) << s;
      }
    }
  }
}
