/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#ifndef AVOGADRO_QTPLUGINS_SPACEGROUP_H
#define AVOGADRO_QTPLUGINS_SPACEGROUP_H

#include <avogadro/qtgui/extensionplugin.h>

#include <memory>

namespace Avogadro {
namespace QtPlugins {

/**
 * @brief Space group features for crystals.
 */
class SpaceGroup : public Avogadro::QtGui::ExtensionPlugin
{
  Q_OBJECT
public:
  explicit SpaceGroup(QObject* parent_ = nullptr);
  ~SpaceGroup() override;

  QString name() const override { return tr("SpaceGroup"); }
  QString description() const override;
  QList<QAction*> actions() const override;
  QStringList menuPath(QAction*) const override;

  bool handleCommand(const QString& command,
                     const QVariantMap& options) override;

  void registerCommands() override;

public slots:
  void setMolecule(QtGui::Molecule* mol) override;

  void moleculeChanged(unsigned int changes);

private slots:
  void updateActions();

  void perceiveSpaceGroup();
  void reduceToPrimitive();
  void conventionalizeCell();
  void symmetrize();
  void fillUnitCell();
  void reduceToAsymmetricUnit();
  void setTolerance();

private:
  // Pop up a dialog box and ask the user to select a space group.
  // Returns the hall number for the selected space group.
  // Returns 0 if the user canceled.
  unsigned short selectSpaceGroup();
  QList<QAction*> m_actions;
  QtGui::Molecule* m_molecule;
  double m_spgTol;

  std::unique_ptr<QAction> m_perceiveSpaceGroupAction;
  std::unique_ptr<QAction> m_reduceToPrimitiveAction;
  std::unique_ptr<QAction> m_conventionalizeCellAction;
  std::unique_ptr<QAction> m_symmetrizeAction;
  std::unique_ptr<QAction> m_fillUnitCellAction;
  std::unique_ptr<QAction> m_reduceToAsymmetricUnitAction;
  std::unique_ptr<QAction> m_setToleranceAction;
};

inline QString SpaceGroup::description() const
{
  return tr("Space group features for crystals.");
}

} // namespace QtPlugins
} // namespace Avogadro

#endif // AVOGADRO_QTPLUGINS_SPACEGROUP_H
