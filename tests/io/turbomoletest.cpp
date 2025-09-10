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

using Avogadro::Core::Molecule;
using Avogadro::Io::TurbomoleFormat;
using namespace std::string_literals;

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
