/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

// #include "qtguitests.h"
#include <gtest/gtest.h>

#include <avogadro/qtplugins/crystal/unitcelldialog.h>

// #include <QtTest/QSignalSpy>

// #include <QtWidgets/QApplication>
// #include <QtWidgets/QLineEdit>

// #include <QtCore/QVariant>

#include <avogadro/core/unitcell.h>

using Avogadro::Core::UnitCell;

class CrystalTest : public testing::Test
{
public:
  CrystalTest();

protected:
  UnitCell m_testUnitCell;
};

CrystalTest::CrystalTest()
{
  m_testUnitCell.setCellParameters(3.0, 3.0, 3.0, 90.0, 90.0, 90.0);
}

TEST_F(CrystalTest, UnitCellEditor)
{
  Avogadro::QtPlugins::UnitCellDialog xx;
}
