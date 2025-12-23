/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include <gtest/gtest.h>

#include <avogadro/rendering/billboardquadstrategy.h>
#include <avogadro/rendering/camera.h>

#include <avogadro/core/array.h>
#include <avogadro/core/vector.h>

typedef Avogadro::Rendering::BillboardQuadStrategy Strategy;

using Avogadro::Vector2f;
using Avogadro::Vector3f;
using Avogadro::Core::Array;
using Avogadro::Rendering::Camera;

TEST(BillboardQuadStrategyTest, exercise)
{
  Strategy s;

  Vector3f anchor(9.3f, 2.1f, -1.8f);
  s.setAnchor(anchor);
  EXPECT_EQ(anchor, s.anchor());

  float radius = 3.4f;
  s.setRadius(radius);
  EXPECT_EQ(radius, s.radius());

  bool useCameraUp = false;
  s.setUseCameraUp(useCameraUp);
  EXPECT_EQ(useCameraUp, s.useCameraUp());

  Vector3f up(Vector3f(1.f, 5.f, 7.f).normalized());
  s.setUp(up);
  EXPECT_EQ(up, s.up());

  Vector2f dims(501.2f, 612.5f);
  s.setDimensions(dims);
  EXPECT_EQ(dims, s.dimensions());

  Strategy::HAlign hAlign(Strategy::HRight);
  s.setHAlign(hAlign);
  EXPECT_EQ(hAlign, s.hAlign());

  Strategy::VAlign vAlign(Strategy::VBottom);
  s.setVAlign(vAlign);
  EXPECT_EQ(vAlign, s.vAlign());

  Camera camera;
  camera.setIdentity();
  camera.rotate(136.2f, Vector3f(205.f, 603.f, -141.f).normalized());
  camera.translate(Vector3f(-13.2f, 15.6f, 102.f));
  Array<Vector3f> quad(s.quad(camera));

  ASSERT_EQ(4, quad.size());

  EXPECT_DOUBLE_EQ(243.83968, quad[0][0]);
  EXPECT_DOUBLE_EQ(-185.99776, quad[0][1]);
  EXPECT_DOUBLE_EQ(730.30627, quad[0][2]);

  EXPECT_DOUBLE_EQ(389.97992, quad[1][0]);
  EXPECT_DOUBLE_EQ(194.00380, quad[1][1]);
  EXPECT_DOUBLE_EQ(437.99942, quad[1][2]);

  EXPECT_DOUBLE_EQ(-134.35634, quad[2][0]);
  EXPECT_DOUBLE_EQ(-379.83984, quad[2][1]);
  EXPECT_DOUBLE_EQ(289.22885, quad[2][2]);

  EXPECT_DOUBLE_EQ(11.783905, quad[3][0]);
  EXPECT_DOUBLE_EQ(0.16171265, quad[3][1]);
  EXPECT_DOUBLE_EQ(-3.0780029, quad[3][2]);

  // Test the useCameraUp options:
  useCameraUp = true;
  s.setUseCameraUp(useCameraUp);
  EXPECT_EQ(useCameraUp, s.useCameraUp());

  quad = s.quad(camera);

  ASSERT_EQ(4, quad.size());

  EXPECT_DOUBLE_EQ(538.01459, quad[0][0]);
  EXPECT_DOUBLE_EQ(371.01038, quad[0][1]);
  EXPECT_DOUBLE_EQ(457.25916, quad[0][2]);

  EXPECT_DOUBLE_EQ(387.27307, quad[1][0]);
  EXPECT_DOUBLE_EQ(484.04947, quad[1][1]);
  EXPECT_DOUBLE_EQ(-7.1766663, quad[1][2]);

  EXPECT_DOUBLE_EQ(162.52542, quad[2][0]);
  EXPECT_DOUBLE_EQ(-112.87738, quad[2][1]);
  EXPECT_DOUBLE_EQ(461.35785, quad[2][2]);

  EXPECT_DOUBLE_EQ(11.783905, quad[3][0]);
  EXPECT_DOUBLE_EQ(0.16171265, quad[3][1]);
  EXPECT_DOUBLE_EQ(-3.0779662, quad[3][2]);

  // Rotating the camera should update the quad:
  camera.preRotate(36.f, Vector3f::UnitY());

  quad = s.quad(camera);

  ASSERT_EQ(4, quad.size());

  EXPECT_DOUBLE_EQ(727.31494, quad[0][0]);
  EXPECT_DOUBLE_EQ(218.08212, quad[0][1]);
  EXPECT_DOUBLE_EQ(-255.12888, quad[0][2]);

  EXPECT_DOUBLE_EQ(383.45709, quad[1][0]);
  EXPECT_DOUBLE_EQ(486.99631, quad[1][1]);
  EXPECT_DOUBLE_EQ(-8.8598633, quad[1][2]);

  EXPECT_DOUBLE_EQ(351.82578, quad[2][0]);
  EXPECT_DOUBLE_EQ(-265.80563, quad[2][1]);
  EXPECT_DOUBLE_EQ(-251.03018, quad[2][2]);

  EXPECT_DOUBLE_EQ(7.967926, quad[3][0]);
  EXPECT_DOUBLE_EQ(3.108551, quad[3][1]);
  EXPECT_DOUBLE_EQ(-4.7611632, quad[3][2]);
}
