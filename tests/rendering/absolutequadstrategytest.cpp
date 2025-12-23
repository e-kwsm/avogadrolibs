/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include <gtest/gtest.h>

#include <avogadro/rendering/absolutequadstrategy.h>
#include <avogadro/rendering/camera.h>

#include <avogadro/core/array.h>
#include <avogadro/core/vector.h>

typedef Avogadro::Rendering::AbsoluteQuadStrategy Strategy;

using Avogadro::Vector2f;
using Avogadro::Vector3f;
using Avogadro::Core::Array;
using Avogadro::Rendering::Camera;

TEST(AbsoluteQuadStrategyTest, exercise)
{
  Strategy s;

  Vector2f dims(501.2f, 612.5f);
  s.setDimensions(dims);
  EXPECT_EQ(dims, s.dimensions());

  Strategy::HAlign hAlign(Strategy::HRight);
  s.setHAlign(hAlign);
  EXPECT_EQ(hAlign, s.hAlign());

  Strategy::VAlign vAlign(Strategy::VBottom);
  s.setVAlign(vAlign);
  EXPECT_EQ(vAlign, s.vAlign());

  Vector3f normal(Vector3f(3.f, 4.f, 5.f).normalized());
  s.setNormal(normal);
  EXPECT_EQ(normal, s.normal());

  Vector3f up(Vector3f(1.f, 5.f, 7.f).normalized());
  s.setUp(up);
  EXPECT_EQ(up, s.up());

  Vector3f anchor(9.3f, 2.1f, -1.8f);
  s.setAnchor(anchor);
  EXPECT_EQ(anchor, s.anchor());

  Camera camera; // dummy, doesn't affect resulting quad.
  Array<Vector3f> quad(s.quad(camera));

  ASSERT_EQ(4, quad.size());

  EXPECT_DOUBLE_EQ(-460.86874, quad[0][0]);
  EXPECT_DOUBLE_EQ(-326.70682, quad[0][1]);
  EXPECT_DOUBLE_EQ(543.34692, quad[0][2]);

  EXPECT_DOUBLE_EQ(-537.40002, quad[1][0]);
  EXPECT_DOUBLE_EQ(81.459717, quad[1][1]);
  EXPECT_DOUBLE_EQ(262.73242, quad[1][2]);

  EXPECT_DOUBLE_EQ(85.831268, quad[2][0]);
  EXPECT_DOUBLE_EQ(-406.06653, quad[2][1]);
  EXPECT_DOUBLE_EQ(278.81454, quad[2][2]);

  EXPECT_DOUBLE_EQ(9.2999878, quad[3][0]);
  EXPECT_DOUBLE_EQ(2.1000061, quad[3][1]);
  EXPECT_DOUBLE_EQ(-1.7999573, quad[3][2]);
}
