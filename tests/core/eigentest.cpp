/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include <Eigen/src/Core/Matrix.h>
#include <gtest/gtest.h>

TEST(EigenTest, vector3d)
{
  Eigen::Vector3d vec;
  vec.setZero();
  EXPECT_EQ(vec.x(), 0);
  EXPECT_EQ(vec.y(), 0);
  EXPECT_EQ(vec.z(), 0);
}
