/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#ifndef AVOGADRO_QTPLUGINS_GAMESSINPUT_H
#define AVOGADRO_QTPLUGINS_GAMESSINPUT_H

#include <avogadro/qtgui/extensionplugin.h>

class QAction;
class QDialog;

namespace Avogadro {
namespace Io {
class FileFormat;
}
namespace MoleQueue {
class JobObject;
}

namespace QtPlugins {

class GamessInputDialog;

class GamessInput : public QtGui::ExtensionPlugin
{
  Q_OBJECT

public:
  explicit GamessInput(QObject* parent = nullptr);
  ~GamessInput() override;

  QString name() const override { return tr("GAMESS input"); }

  QString description() const override
  {
    return tr("Generate input for GAMESS.");
  }

  QList<QAction*> actions() const override;

  QStringList menuPath(QAction*) const override;

  void setMolecule(QtGui::Molecule* mol) override;

public slots:
  /**
   * Emitted when the user requests that a job's output be loaded in Avogadro.
   */
  void openJobOutput(const MoleQueue::JobObject& job);

  bool readMolecule(QtGui::Molecule& mol) override;

private slots:
  void menuActivated();

private:
  QAction* m_action;
  QtGui::Molecule* m_molecule;
  GamessInputDialog* m_dialog;
  const Io::FileFormat* m_outputFormat;
  QString m_outputFileName;
};
} // namespace QtPlugins
} // namespace Avogadro

#endif // AVOGADRO_QTPLUGINS_GAMESSINPUT_H
