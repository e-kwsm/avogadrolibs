/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#ifndef OPENMMINPUTDIALOG_H
#define OPENMMINPUTDIALOG_H

#include "ui_openmminputdialog.h"

#include <QHash>
#include <QtCore/QSettings>

class QJsonObject;
class QTextEdit;

namespace Avogadro {
namespace QtGui {
class Molecule;
}

namespace QtPlugins {
class OpenMMInputDialog : public QDialog
{
  Q_OBJECT

public:
  explicit OpenMMInputDialog(QWidget* parent = nullptr,
                             Qt::WindowFlags flag = Qt::WindowFlags());
  ~OpenMMInputDialog() override;

  void readSettings(QSettings&);
  void writeSettings(QSettings&) const;

  enum forceFieldType
  {
    amber96,
    amber99sb,
    amber99sbildn,
    amber99sbnmr,
    amber03,
    amber10
  };
  enum waterModelType
  {
    spce,
    tip3p,
    tip4pew,
    tip5p,
    implicit
  };
  enum nonBondedType
  {
    NoCutoff,
    CutoffNonPeriodic,
    CutoffPeriodic,
    Ewald,
    PME
  };
  enum constraintType
  {
    None,
    HBonds,
    AllBonds,
    HAngles
  };
  enum integratorType
  {
    Langevin,
    Verlet,
    Brownian,
    VariableLangevin,
    VariableVerlet
  };
  enum barostatType
  {
    NoBarostat,
    MonteCarlo
  };
  enum platformType
  {
    Reference,
    OpenCL,
    CPU,
    CUDA
  };
  enum precisionType
  {
    singlePrecision,
    mixedPrecision,
    doublePrecision
  };
  void setMolecule(QtGui::Molecule* molecule);

protected:
  /**
   * Reimplemented to update the dialog when it is shown
   */
  void showEvent(QShowEvent* event) override;

private:
  Ui::OpenMMInputDialog ui;
  QtGui::Molecule* m_molecule = nullptr;

  // QString m_title;
  QString m_readData;
  forceFieldType m_forceFieldType = amber99sbildn;
  QString m_title;
  QString m_savePath;
  waterModelType m_waterModelType = tip3p;
  nonBondedType m_nonBondedType = PME;
  constraintType m_constraintType = HBonds;
  integratorType m_integratorType = Langevin;
  barostatType m_barostatType = NoBarostat;

  int m_deviceIndex = 1;
  int m_openclPlatformIndex = 1;
  int m_rigidWater = 0;
  double m_temperature = 298.15;
  double m_generationTemperature = 298.15;
  double m_nonBondedCutoff = 1.0;
  double m_timeStep = 2.0;
  double m_ewaldTolerance = 0.0005;
  double m_constraintTolerance = 0.00001;
  int m_reportInterval = 1000;
  int m_equilibriationSteps = 100;
  int m_productionSteps = 1000;
  double m_errorTolerance = 0.0001;
  double m_collisionRate = 1.0;
  double m_pressure = 1.0;
  int m_barostatInterval = 25;
  QString m_dumpXYZ;
  int m_dumpStep = 1;
  int m_velocityDistRandom = 0;
  platformType m_platformType = CUDA;
  precisionType m_precisionType = mixedPrecision;
  int m_thermoInterval = 50;
  int m_minimize = 0;
  int m_minimizeSteps = 1000;
  bool m_DCDReporter = true;
  bool m_PDBReporter = false;
  bool m_stateDataReporter = true;
  bool m_stepIndex;
  bool m_time;
  bool m_speed;
  bool m_progress;
  bool m_potentialEnergy;
  bool m_kineticEnergy;
  bool m_totalEnergy;
  bool m_temperatureCheck;
  bool m_volume;
  bool m_density;

  QString m_output;
  bool m_dirty = false;
  bool m_warned = false;
  bool readData = false;

  QTextEdit* m_jobEdit = nullptr;
  QTextEdit* m_moleculeEdit = nullptr;
  QString m_inputCoordFileName;
  QString m_topologyFileName;
  QString m_jobFileName;

  // Generate an input deck as a string
  QString generateInputDeck();
  // Translate enums to strings
  QString getForceFieldType(forceFieldType t);
  QString getImplicitSolventType(forceFieldType t);
  QString getConstraintType(constraintType t);
  QString getWaterModelType(waterModelType t);
  QString getNonBondedType(nonBondedType t);
  QString getIntegratorType(integratorType t);
  QString getBarostatType(barostatType t);
  QString getRigidWater(int t);
  QString getVelocityDistRandom(int t);
  QString getPlatformType(platformType t);
  QString getPrecisionType(precisionType t);
  QString getMinimize(int t);

  // Enable/disable form elements
  void deckDirty(bool);
  void addMoleculeDataTab();

public Q_SLOTS:
  void updatePreviewText();

private Q_SLOTS:
  //! Button Slots
  void textEditModified();
  void resetClicked();
  void generateClicked();
  void enableFormClicked();

  void setScriptName();
  void setInputCoordName();
  void setTopologyName();

  void setForceField(int);
  void setConstraintType(int);
  void setWaterModelType(int);
  void setNonBondedType(int);

  void setIntegratorType(int);
  void setBarostatType(int);

  void setRigidWater(int);
  void setTemperature(double);
  void setGenerationTemperature(double);
  void setNonBondedCutoff(double);

  void setTimeStep(double);
  void setEwaldTolerance(double);
  void setConstraintTolerance(double);
  void setReportInterval(int);
  void setEquilibriationSteps(int);
  void setProductionSteps(int);
  void setDeviceIndex(int);
  void setOpenCLPlatformIndex(int);
  void setErrorTolerance(double);
  void setCollisionRate(double);
  void setPressure(double);
  void setBarostatInterval(int);

  void setVelocityDistRandom(int);
  void setDCDReporter(bool);
  void setPDBReporter(bool);
  void setStateDataReporter(bool);
  void setStepIndexBoolean(bool);
  void setTimeBoolean(bool);
  void setSpeedBoolean(bool);
  void setProgressBoolean(bool);
  void setPotentialEnergyBoolean(bool);
  void setKineticEnergyBoolean(bool);
  void setTotalEnergyBoolean(bool);
  void setTemperatureBoolean(bool);
  void setVolumeBoolean(bool);
  void setDensityBoolean(bool);
  void setPlatformType(int);
  void setPrecisionType(int);
  void setMinimize(int);
  void setMinimizeSteps(int);
};
} // namespace QtPlugins
} // namespace Avogadro

#endif
