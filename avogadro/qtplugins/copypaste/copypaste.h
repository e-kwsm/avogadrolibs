/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#ifndef AVOGADRO_QTPLUGINS_COPYPASTE_H
#define AVOGADRO_QTPLUGINS_COPYPASTE_H

#include <avogadro/core/avogadrocore.h>
#include <avogadro/qtgui/extensionplugin.h>

#include <QtGui/QIcon>

#include <memory>

namespace Avogadro {
namespace Io {
class FileFormat;
}

namespace QtPlugins {

/**
 * @brief The CopyPaste class allows interaction with the system clipboard.
 */
class CopyPaste : public QtGui::ExtensionPlugin
{
  Q_OBJECT
public:
  explicit CopyPaste(QObject* parent_ = nullptr);
  ~CopyPaste() override;

  QString name() const override { return tr("Copy and paste"); }

  QString description() const override
  {
    return tr("Interact with the clipboard.");
  }

  QList<QAction*> actions() const override;

  QStringList menuPath(QAction* action) const override;

public slots:
  void setMolecule(QtGui::Molecule* mol) override;

private slots:
  bool copy(
    Io::FileFormat* format); // returns bool so cut can reuse implementation.
  bool copyCJSON();
  void copySMILES();
  void copyInChI();
  void copyXYZ();
  void cut();
  void paste();
  void clear();

private:
  // Cached between emitting moleculeReady() and calling readMolecule().
  QByteArray m_pastedData;
  Io::FileFormat* m_pastedFormat;

  QtGui::Molecule* m_molecule;

  std::unique_ptr<QAction> m_copyAction;
  std::unique_ptr<QAction> m_copySMILES;
  std::unique_ptr<QAction> m_copyInChI;
  std::unique_ptr<QAction> m_copyXYZ;
  std::unique_ptr<QAction> m_cutAction;
  std::unique_ptr<QAction> m_clearAction;
  std::unique_ptr<QAction> m_pasteAction;
};

} // namespace QtPlugins
} // namespace Avogadro

#endif // AVOGADRO_QTPLUGINS_COPYPASTE_H
