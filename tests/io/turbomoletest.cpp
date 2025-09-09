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

         R"($periodic 1
$cell
6.0
$end
)"s,

         R"($periodic 2
$cell
  6.0 8.0 90.0
$end
)"s,

         R"($periodic 3
$cell
  6.0 8.0 10.0 90.0 89.0 78.0
$end
)"s,

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

         R"($periodic
$end
)"s,

         R"($periodic 1 2
$end
)"s,

         R"($periodic -1
$end
)"s,

         R"($periodic 4
$end
)"s,

               R"($periodic 1
$cell
6.0 10.0
$end
)"s,

       }) {
    TurbomoleFormat tmol;
    Molecule molecule;
    EXPECT_FALSE(tmol.readString(s, molecule)) << s;
  }
}
