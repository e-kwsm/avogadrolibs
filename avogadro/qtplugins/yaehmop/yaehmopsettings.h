/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#ifndef AVOGADRO_QTPLUGINS_YAEHMOPSETTINGS_H
#define AVOGADRO_QTPLUGINS_YAEHMOPSETTINGS_H

#include <QString>

namespace Avogadro::QtPlugins {

static const char* YAEHMOP_DEFAULT_SPECIAL_KPOINTS = "GM 0 0 0";

struct YaehmopSettings
{
  YaehmopSettings() = default;

  unsigned long long numBandKPoints = 40;
  QString specialKPoints = YAEHMOP_DEFAULT_SPECIAL_KPOINTS;
  bool displayYaehmopInput = false;
  bool limitY = false;
  double minY = 0.0;
  double maxY = 0.0;
  bool plotFermi = false;
  double fermi = 0.0;
  bool zeroFermi = false;
  unsigned short numDim = 3;
};

} // namespace Avogadro::QtPlugins

#endif // AVOGADRO_QTPLUGINS_YAEHMOPSETTINGS_H
