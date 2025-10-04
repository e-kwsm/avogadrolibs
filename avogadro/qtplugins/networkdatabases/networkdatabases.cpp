/******************************************************************************
  This source file is part of the Avogadro project.
  This source code is released under the 3-Clause BSD License, (see "LICENSE").
******************************************************************************/

#include "networkdatabases.h"

#include <avogadro/io/fileformatmanager.h>
#include <avogadro/qtgui/molecule.h>

#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QAction>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>

#include <cmath>

namespace Avogadro::QtPlugins {

NetworkDatabases::NetworkDatabases(QObject* parent_)
  : ExtensionPlugin(parent_), m_action(new QAction(this))
{
  m_action->setEnabled(true);
  m_action->setText("Download by &Name…");
  m_action->setProperty("menu priority", 190);
  connect(m_action, SIGNAL(triggered()), SLOT(showDialog()));
}

NetworkDatabases::~NetworkDatabases() {}

QList<QAction*> NetworkDatabases::actions() const
{
  return QList<QAction*>() << m_action;
}

QStringList NetworkDatabases::menuPath(QAction*) const
{
  return QStringList() << tr("&File") << tr("&Import");
}

void NetworkDatabases::setMolecule(QtGui::Molecule* mol)
{
  m_molecule = mol;
}

bool NetworkDatabases::readMolecule(QtGui::Molecule& mol)
{
  if (m_moleculeData.isEmpty() || m_moleculeName.isEmpty())
    return false;

  bool readOK = Io::FileFormatManager::instance().readString(
    mol, m_moleculeData.data(), "sdf");
  if (readOK) // worked, so set the filename
    mol.setData("name", m_moleculeName.toStdString());

  return readOK;
}

void NetworkDatabases::showDialog()
{
  if (!m_network) {
    m_network = new QNetworkAccessManager(this);
    connect(m_network, SIGNAL(finished(QNetworkReply*)), this,
            SLOT(replyFinished(QNetworkReply*)));
  }
  // Prompt for a chemical structure name
  bool ok;
  QString structureName = QInputDialog::getText(
    qobject_cast<QWidget*>(parent()), tr("Chemical Name"),
    tr("Chemical structure to download."), QLineEdit::Normal, "", &ok);

  if (!ok || structureName.isEmpty())
    return;

  // Hard coding the NIH resolver download URL - this could be used for other
  // services
  m_network->get(
    QNetworkRequest(QUrl("https://cactus.nci.nih.gov/chemical/structure/" +
                         structureName + "/file?format=sdf&get3d=true")));

  m_moleculeName = structureName;
  m_triedPubChem = false;
  if (!m_progressDialog) {
    m_progressDialog = new QProgressDialog(qobject_cast<QWidget*>(parent()));
  }
  m_progressDialog->setLabelText(tr("Querying for %1").arg(structureName));
  m_progressDialog->setRange(0, 0);
  m_progressDialog->show();
}

void NetworkDatabases::replyFinished(QNetworkReply* reply)
{
  m_progressDialog->hide();
  // Read in all the data
  if (!reply->isReadable()) {
    QMessageBox::warning(qobject_cast<QWidget*>(parent()),
                         tr("Network Download Failed"),
                         tr("Network timeout or other error."));
    reply->deleteLater();
    return;
  }

  QByteArray data = reply->readAll();
  reply->deleteLater();

  bool isError = data.contains("Error report") ||
                 data.contains("Page not found (404)") ||
                 data.contains("PUGREST.NotFound") || data.isEmpty();

  // Cactus sometimes returns 2D coordinates even with get3d=true (e.g.
  // cholesterol). If so, fall back to PubChem's 3D record endpoint.
  if (!m_triedPubChem) {
    if (!isError && sdfHasThreeDCoordinates(data)) {
      m_moleculeData = data;
      emit moleculeReady(1);
      return;
    }

    m_triedPubChem = true;
    QByteArray encoded = QUrl::toPercentEncoding(m_moleculeName);
    QUrl pubchemUrl(
      QStringLiteral("https://pubchem.ncbi.nlm.nih.gov/rest/pug/compound/"
                     "name/") +
      QString::fromUtf8(encoded) + QStringLiteral("/SDF?record_type=3d"));
    m_network->get(QNetworkRequest(pubchemUrl));
    m_progressDialog->setLabelText(
      tr("Querying PubChem for %1").arg(m_moleculeName));
    m_progressDialog->setRange(0, 0);
    m_progressDialog->show();
    return;
  }

  // Second pass: response from PubChem.
  m_triedPubChem = false;
  if (isError || !sdfHasThreeDCoordinates(data)) {
    QMessageBox::warning(
      qobject_cast<QWidget*>(parent()), tr("Network Download Failed"),
      tr("Specified molecule could not be found: %1").arg(m_moleculeName));
    return;
  }

  m_moleculeData = data;
  emit moleculeReady(1);
}

bool NetworkDatabases::sdfHasThreeDCoordinates(const QByteArray& data)
{
  // MDL MOL/SDF layout: line 0 title, line 1 metadata, line 2 comment,
  // line 3 counts ("aaabbb..."), then `aaa` atom lines with x/y/z in
  // columns 0-9, 10-19, 20-29.
  QList<QByteArray> lines = data.split('\n');
  if (lines.size() < 5)
    return false;

  bool ok = false;
  int atomCount = lines[3].left(3).trimmed().toInt(&ok);
  if (!ok || atomCount <= 0)
    return false;
  if (lines.size() < 4 + atomCount)
    return false;

  for (int i = 0; i < atomCount; ++i) {
    const QByteArray& atomLine = lines[4 + i];
    if (atomLine.size() < 30)
      continue;
    double z = atomLine.mid(20, 10).trimmed().toDouble(&ok);
    if (ok && std::fabs(z) > 1e-4)
      return true;
  }
  return false;
}
} // namespace Avogadro::QtPlugins
