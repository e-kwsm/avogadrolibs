/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#ifndef AVOGADRO_QTGUI_SCENEPLUGIN_H
#define AVOGADRO_QTGUI_SCENEPLUGIN_H

#include "avogadroqtguiexport.h"

#include "pluginlayermanager.h"
#include <avogadro/core/avogadrocore.h>
#include <avogadro/qtplugins/pluginfactory.h>

#include <QtCore/QObject>

namespace Avogadro {

namespace Core {
class Molecule;
}

namespace Rendering {
class GroupNode;
}

namespace QtGui {

class RWMolecule;
class Molecule;

/**
 * @class ScenePluginFactory sceneplugin.h <avogadro/qtgui/sceneplugin.h>
 * @brief The base class for scene plugin factories in Avogadro.
 * @author Marcus D. Hanwell
 */
class AVOGADROQTGUI_EXPORT ScenePlugin : public QObject
{
  Q_OBJECT

public:
  explicit ScenePlugin(QObject* parent = nullptr);
  ~ScenePlugin() override;

  /**
   * Process the supplied atom, and add the necessary primitives to the scene.
   */
  virtual void process(const QtGui::Molecule& molecule,
                       Rendering::GroupNode& node);

  virtual void processEditable(const RWMolecule& molecule,
                               Rendering::GroupNode& node);

  /**
   * The name of the scene plugin, will be displayed in the user interface.
   */
  virtual QString name() const = 0;

  /**
   * A description of the scene plugin, may be displayed in the user interface.
   */
  virtual QString description() const = 0;

  /**
   * Returns true if the scene plugin has been enabled and is active.
   */
  virtual bool isEnabled() const;

  /**
   * Returns true if the scene plugin has been enabled and is active in the
   * active scene.
   */
  virtual bool isActiveLayerEnabled() const;

  /**
   * Set the enabled state of the plugin (default should be false).
   */
  virtual void setEnabled(bool enable);

  /**
   * @return whether this plugin works with the current molecule.
   * This is used to determine if the plugin should be enabled in the UI.
   * For example, a plugin that only works with unit cells would return
   * false if the current molecule does not include a unit cell.
   */
  virtual bool isApplicable() const { return true; }

  /**
   * @return the widget for plugin settings (e.g., colors, widths, etc.)
   */
  virtual QWidget* setupWidget();
  /**
   * This method exists to query without creating the widget.
   * @return true if the plugin has a setup widget
   */
  virtual bool hasSetupWidget() const { return false; }

  /**
   * Returns if this plugin should be considered in the default behavior,
   * or it should reset to true or false.
   */
  enum DefaultBehavior
  {
    Ignore,
    False,
    True
  };
  virtual DefaultBehavior defaultBehavior() const { return Ignore; }

signals:
  void drawablesChanged();

protected:
  PluginLayerManager m_layerManager;
};

/**
 * @class ScenePluginFactory sceneplugin.h <avogadro/qtgui/sceneplugin.h>
 * @brief The base class for scene plugin factories in Avogadro.
 * @author Marcus D. Hanwell
 */
class AVOGADROQTGUI_EXPORT ScenePluginFactory
  : public Avogadro::QtPlugins::PluginFactory<ScenePlugin>
{
public:
  ~ScenePluginFactory() override {}
};

} // namespace QtGui
} // namespace Avogadro

Q_DECLARE_INTERFACE(Avogadro::QtGui::ScenePluginFactory,
                    "org.openchemistry.avogadro.ScenePluginFactory")

#endif // AVOGADRO_QTGUI_SCENEPLUGIN_H
