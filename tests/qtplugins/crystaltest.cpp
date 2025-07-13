/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

// #include "qtguitests.h"
#include <gtest/gtest.h>

#include <avogadro/qtplugins/crystal/unitcelldialog.h>

#include <QtTest/QSignalSpy>

// #include <QtWidgets/QApplication>
// #include <QtWidgets/QLineEdit>

// #include <QtCore/QVariant>

#include <avogadro/core/unitcell.h>

using Avogadro::Core::UnitCell;

// Need a QApplication to instantiate widget
#define START_QAPP                                                             \
  int argc = 1;                                                                \
  char argName[] = "FakeApp.exe";                                              \
  char* argv[2] = { argName, nullptr };                                        \
  QApplication app(argc, argv);                                                \
  Q_UNUSED(app)

TEST(FileBrowseWidgetTest, setFileName)
{
  START_QAPP;

#ifndef HOGE
  using Avogadro::QtPlugins::UnitCellDialog;
  UnitCellDialog dialog;
  if (QT_VERSION == 6)
    QSignalSpy spy(&dialog, &UnitCellDialog::parametersEdited);
  else
    QSignalSpy spy(&dialog, SIGNAL(&UnitCellDialog::parametersEdited));

#else
  FileBrowseWidget widget;
  if (QT_VERSION == 6)
    QSignalSpy spy(&widget, &FileBrowseWidget::fileNameChanged);
  else
    QSignalSpy spy(&widget, SIGNAL(fileNameChanged(QString)));
  widget.setFileName("some file");
  EXPECT_EQ(1, spy.count());
  EXPECT_STREQ("some file", qPrintable(spy.front().front().toString()));
  EXPECT_STREQ("some file", qPrintable(widget.fileName()));
  EXPECT_STREQ("some file", qPrintable(widget.lineEdit()->text()));
#endif
}

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
