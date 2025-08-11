/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include <gtest/gtest.h>

#include <avogadro/core/variantmap.h>

using Avogadro::Core::VariantMap;

TEST(VariantMapTest, size)
{
  VariantMap map;
  EXPECT_EQ(map.size(), static_cast<size_t>(0));
}

TEST(VariantMapTest, isEmpty)
{
  VariantMap map;
  EXPECT_TRUE(map.isEmpty());

  map.setValue("value1", 1);
  EXPECT_FALSE(map.isEmpty());
  EXPECT_TRUE(map.hasValue("value1"));
  EXPECT_FALSE(map.hasValue("value2"));
  EXPECT_EQ(map.value("value1").toInt(), 1);
}
