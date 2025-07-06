/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include "scriptcharges.h"

#include "scriptchargemodel.h"

#include <avogadro/calc/chargemanager.h>
#include <avogadro/calc/chargemodel.h>

#include <avogadro/qtgui/scriptloader.h>
#include <avogadro/qtgui/utilities.h>

#include <QtCore/QDebug>

namespace Avogadro::QtPlugins {

ScriptCharges::ScriptCharges(QObject* p) : ExtensionPlugin(p)
{
  refreshModels();
}

ScriptCharges::~ScriptCharges() {}

QList<QAction*> ScriptCharges::actions() const
{
  return QList<QAction*>();
}

QStringList ScriptCharges::menuPath(QAction*) const
{
  return QStringList();
}

void ScriptCharges::setMolecule(QtGui::Molecule*) {}

void ScriptCharges::refreshModels()
{
  unregisterModels();
  m_models.clear();

  QMultiMap<QString, QString> scriptPaths =
    QtGui::ScriptLoader::scriptList("charges");
  foreach (const QString& filePath, scriptPaths) {
    auto model = std::make_shared<ScriptChargeModel>(filePath);
    if (model->isValid())
      m_models.push_back(std::move(model));
  }

  registerModels();
}

void ScriptCharges::unregisterModels()
{
  for (const auto& it : m_models) {
    Calc::ChargeManager::unregisterModel(it->identifier());
  }
}

void ScriptCharges::registerModels()
{
  for (auto& it : m_models) {
    if (!Calc::ChargeManager::registerModel(it->newInstance())) {
      qDebug() << "Could not register model" << it->identifier().c_str()
               << "due to name conflict.";
    }
  }
}

} // namespace Avogadro::QtPlugins
