#include "WAirplanes.h"
#include "ui_WAirplanes.h"

#include <QtSql>
#include <QtWidgets>

#include "Database.h"
#include "DirtyProxyModel.h"

WAirplanes::WAirplanes(QWidget *parent) :
    WBase(parent),
    m_ui(new Ui::WAirplanes)
{
  m_ui->setupUi(this);
  setupModels();
  setupHandlers();
}

WAirplanes::~WAirplanes(void)
{
  if (m_ui)
  {
    delete m_ui;
  }
}

void WAirplanes::setupHandlers(void)
{
  connect(m_ui->btnSave, &QPushButton::clicked, this, &WAirplanes::save);
  connect(m_ui->btnAdd, &QPushButton::clicked, this, &WAirplanes::add);
  connect(m_ui->btnDelete, &QPushButton::clicked, this, &WAirplanes::remove);

  connect(m_ui->txtModel, &QLineEdit::textEdited, [&]() { setDirty(true); });
  connect(m_ui->txtRegistration, &QLineEdit::textEdited, [&]() { setDirty(true); });
  connect(m_ui->cbClass, &QComboBox::activated, [&]() { setDirty(true); });

  connect(
      m_ui->tblAirplanes->selectionModel(), &QItemSelectionModel::currentChanged, this,
      [&](const QModelIndex &current, const QModelIndex &previous)
      {
        if (m_bEdited && current != previous)
        {
          if (QMessageBox::question(
                  this, tr("Changes not saved"),
                  tr("Changes not saved and they will be discarded, are you sure you wan't to continue?")) !=
              QMessageBox::Yes)
          {
            QSignalBlocker blocker(m_ui->tblAirplanes->selectionModel());
            m_ui->tblAirplanes->selectionModel()->setCurrentIndex(previous, QItemSelectionModel::NoUpdate);
            m_ui->tblAirplanes->selectRow(previous.row());

            return;
          }
          setDirty(false);
        }

        QSqlRecord record = m_model->record(current.row());
        m_sSelectedId = record.value("id").toString();
        m_ui->txtModel->setText(record.value("model").toString());
        m_ui->txtRegistration->setText(record.value("registration").toString());
        m_ui->cbClass->setCurrentIndex(m_ui->cbClass->findText(record.value("airplaneClass").toString()));
        m_ui->btnDelete->setEnabled(true);
      },
      Qt::QueuedConnection);
}

void WAirplanes::setupModels(void)
{
  m_model = new QSqlTableModel(this, getDatabase());

  m_model->setTable("airplanes");
  m_model->setHeaderData(m_model->record().indexOf("id"), Qt::Horizontal, tr("ID"));
  m_model->setHeaderData(m_model->record().indexOf("model"), Qt::Horizontal, tr("Model"));
  m_model->setHeaderData(m_model->record().indexOf("registration"), Qt::Horizontal, tr("Registration"));
  m_model->setHeaderData(m_model->record().indexOf("airplaneClass"), Qt::Horizontal, tr("Class"));
  m_model->select();

  m_proxy = new DirtyProxyModel(this);
  m_proxy->setSourceModel(m_model);

  m_ui->tblAirplanes->setModel(m_proxy);
  m_ui->tblAirplanes->setColumnWidth(0, 25);
  m_ui->tblAirplanes->setColumnHidden(m_model->record().indexOf("id") + 1, true);
}

void WAirplanes::save(void)
{
  bool bIsNew = m_sSelectedId == "";
  QString sId = m_sSelectedId;

  QSqlQuery query(getDatabase());
  QString sql;

  if (bIsNew)
  {
    sql = "INSERT INTO airplanes ("
          "  id,"
          "  model,"
          "  registration,"
          "  airplaneClass"
          ") VALUES ("
          "  :id,"
          "  :model,"
          "  :registration,"
          "  :airplaneClass"
          ")";
    sId = generateId();
  }
  else
  {
    sql = "UPDATE airplanes SET"
          "  id=:id,"
          "  model=:model,"
          "  registration=:registration,"
          "  airplaneClass=:airplaneClass "
          "WHERE id=:id";
  }

  query.prepare(sql);
  query.bindValue(":id", sId);
  query.bindValue(":model", m_ui->txtModel->text());
  query.bindValue(":registration", m_ui->txtRegistration->text());
  query.bindValue(":airplaneClass", m_ui->cbClass->currentText());

  if (!query.exec())
  {
    QString sReg = m_ui->txtRegistration->text();
    qDebug() << "Error while saving airplane: " << query.lastError();
    QMessageBox::warning(this, tr("Error"), tr("Error while saving airplane %1!").arg(sReg));
    return;
  }

  m_sSelectedId = sId;

  QModelIndex index = m_ui->tblAirplanes->currentIndex();
  m_model->select();
  setDirty(false);
  m_ui->tblAirplanes->setCurrentIndex(index);
}

void WAirplanes::add(void)
{
  m_model->select();

  m_bEdited = false;
  m_sSelectedId = "";

  m_ui->btnDelete->setEnabled(false);
  m_ui->txtModel->setText("");
  m_ui->txtRegistration->setText("");
  m_ui->cbClass->setCurrentIndex(0);

  setDirty(false);
}

void WAirplanes::remove(void)
{
  if (m_sSelectedId == "")
  {
    return;
  }

  QString sReg = m_ui->txtRegistration->text();

  if (QMessageBox::critical(this, tr("Delete Airplane"), tr("Are you sure you wan't to delete airplane %1?").arg(sReg),
                            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
  {
    return;
  }

  QSqlQuery query;
  QString sql = "DELETE FROM airplanes WHERE id=:id";

  query.prepare(sql);
  query.bindValue(":id", m_sSelectedId);
  if (!query.exec())
  {
    qDebug() << "Error while removing airplane: " << query.lastError();
    QMessageBox::warning(this, tr("Error"), tr("Error while removing airplane %1!").arg(sReg));
    return;
  }

  add();
}

void WAirplanes::setDirty(bool bDirty)
{
  if (bDirty)
  {
    m_proxy->setDirtyRow(m_ui->tblAirplanes->currentIndex().row());
  }
  else
  {
    m_proxy->setDirtyRow(-1);
  }

  m_bEdited = bDirty;
  emit dirtyChanged(bDirty);
}
