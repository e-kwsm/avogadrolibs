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

  std::map<std::string, std::vector<std::string>> invalid_inputs = {
    {
      "1"s,
      {
        "$cell\nFF"s, "$cell\n6.0 10.0"s,
        // "$cell\n6.0\n10.0"s, // TODO extra line
        "$lattice\nFF"s, "$lattice\n6.0 10.0"s,
        // "$lattice\n6.0\n10.0\n$end"s, // TODO extra line
      },
    },
    {
      "2"s,
      {
        "$cell\n6.0 8.0"s,
        "$cell\n6.0 8.0 90.0 90.0"s,
        // "$cell\n6.0 8.0 90.0\n90.0"s, // TODO extra line
        "$lattice\n6.0\n0.0 8.0"s,
        "$lattice\n6.0 0.0"s,
        "$lattice\n6.0 0.0\n0.0"s,
        "$lattice\n6.0 0.0\n0.0 8.0 0.0"s,
        // "$lattice\n6.0 0.0\n0.0 8.0\n0.0 0.0"s, // TODO extra line
        "$lattice\n6.0 0.0 0.0\n0.0 8.0"s,
      },
    },
    { "3"s,
      {
        "$cell\n6.0 8.0 10.0 90.0 90.0"s,
        "$cell\n6.0 8.0 10.0 90.0 90.0 90.0 90.0"s, "$lattice\n6.0"s,
        "$lattice\n6.0 0.0\n0.0 8.0"s, "$lattice\n6.0 0.0 0.0"s,
        "$lattice\n6.0 0.0 0.0\n0.0 8.0 0.0"s,
        "$lattice\n6.0 0.0 0.0\n0.0 8.0 0.0\n0.0 0.0"s,
        "$lattice\n6.0 0.0 0.0\n0.0 8.0\n0.0 0.0 10.0"s,
        "$lattice\n6.0 0.0 0.0 0.0\n0.0 8.0 0.0\n0.0 0.0 10.0"s,
        // "$lattice\n6.0 0.0 0.0 0.0\n0.0 8.0 0.0\n0.0 0.0 10.0\n1.0"s, // TODO
        // // extra line
      } }
  };

  for (const auto& [periodic, strs] : invalid_inputs) {
    for (const auto& s : strs) {
      for (const auto& [pre, post] : {
             std::make_pair("$periodic "s + periodic + "\n"s, "\n$end"s),
             std::make_pair(""s, "\n$periodic "s + periodic + "\n$end"s),
           }) {
        TurbomoleFormat tmol;
        Molecule molecule;
        auto t = pre + s + post;
        EXPECT_FALSE(tmol.readString(t, molecule)) << t;
      }
    }
  }
}
