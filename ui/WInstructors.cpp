#include "WInstructors.h"
#include "ui_WInstructors.h"

#include <QtSql>
#include <QtWidgets>

#include "Database.h"
#include "DirtyProxyModel.h"

WInstructors::WInstructors(QWidget *parent) :
    WBase(parent),
    m_ui(new Ui::WInstructors)
{
  m_ui->setupUi(this);
  setupModels();
  setupHandlers();
}

WInstructors::~WInstructors(void)
{
  if (m_ui)
  {
    delete m_ui;
  }
}

void WInstructors::setupHandlers(void)
{
  connect(m_ui->btnSave, &QPushButton::clicked, this, &WInstructors::save);
  connect(m_ui->btnAdd, &QPushButton::clicked, this, &WInstructors::add);
  connect(m_ui->btnDelete, &QPushButton::clicked, this, &WInstructors::remove);

  connect(m_ui->txtName, &QLineEdit::textEdited, [&]() { setDirty(true); });
  connect(m_ui->txtSurname, &QLineEdit::textEdited, [&]() { setDirty(true); });
  connect(m_ui->cbLicenceType, &QComboBox::activated, [&]() { setDirty(true); });
  connect(m_ui->txtLicenceNumber, &QLineEdit::textEdited, [&]() { setDirty(true); });

  connect(
      m_ui->tblInstructors->selectionModel(), &QItemSelectionModel::currentChanged, this,
      [&](const QModelIndex &current, const QModelIndex &previous)
      {
        if (m_bEdited && current != previous)
        {
          if (QMessageBox::question(
                  this, tr("Changes not saved"),
                  tr("Changes not saved and they will be discarded, are you sure you wan't to continue?")) !=
              QMessageBox::Yes)
          {
            QSignalBlocker blocker(m_ui->tblInstructors->selectionModel());
            m_ui->tblInstructors->selectionModel()->setCurrentIndex(previous, QItemSelectionModel::NoUpdate);
            m_ui->tblInstructors->selectRow(previous.row());

            return;
          }
          setDirty(false);
        }

        QSqlRecord record = m_model->record(current.row());
        m_sSelectedId = record.value("id").toString();
        m_ui->txtName->setText(record.value("name").toString());
        m_ui->txtSurname->setText(record.value("surname").toString());
        m_ui->cbLicenceType->setCurrentIndex(m_ui->cbLicenceType->findText(record.value("licenceType").toString()));
        m_ui->txtLicenceNumber->setText(record.value("licenceNumber").toString());
        m_ui->btnDelete->setEnabled(true);
      },
      Qt::QueuedConnection);
}

void WInstructors::setupModels(void)
{
  m_model = new QSqlTableModel(this, getDatabase());

  m_model->setTable("instructors");
  m_model->setHeaderData(m_model->record().indexOf("id"), Qt::Horizontal, tr("ID"));
  m_model->setHeaderData(m_model->record().indexOf("name"), Qt::Horizontal, tr("Name"));
  m_model->setHeaderData(m_model->record().indexOf("surname"), Qt::Horizontal, tr("Surname"));
  m_model->setHeaderData(m_model->record().indexOf("licenceType"), Qt::Horizontal, tr("Licence Type"));
  m_model->setHeaderData(m_model->record().indexOf("licenceNumber"), Qt::Horizontal, tr("Licence Number"));
  m_model->select();

  m_proxy = new DirtyProxyModel(this);
  m_proxy->setSourceModel(m_model);

  m_ui->tblInstructors->setModel(m_proxy);
  m_ui->tblInstructors->setColumnWidth(0, 25);
  m_ui->tblInstructors->setColumnWidth(m_model->record().indexOf("licenceNumber") + 1, 200);
  m_ui->tblInstructors->setColumnHidden(m_model->record().indexOf("id") + 1, true);
}

void WInstructors::save(void)
{
  bool bIsNew = m_sSelectedId == "";
  QString sId = m_sSelectedId;

  QSqlQuery query(getDatabase());
  QString sql;

  if (bIsNew)
  {
    sql = "INSERT INTO instructors ("
          "  id,"
          "  name,"
          "  surname,"
          "  licenceType,"
          "  licenceNumber"
          ") VALUES ("
          "  :id,"
          "  :name,"
          "  :surname,"
          "  :licenceType,"
          "  :licenceNumber"
          ")";
    sId = generateId();
  }
  else
  {
    sql = "UPDATE instructors SET"
          "  id=:id,"
          "  name=:name,"
          "  surname=:surname,"
          "  licenceType=:licenceType,"
          "  licenceNumber=:licenceNumber "
          "WHERE id=:id";
  }

  query.prepare(sql);
  query.bindValue(":id", sId);
  query.bindValue(":name", m_ui->txtName->text());
  query.bindValue(":surname", m_ui->txtSurname->text());
  query.bindValue(":licenceType", m_ui->cbLicenceType->currentText());
  query.bindValue(":licenceNumber", m_ui->txtLicenceNumber->text());

  if (!query.exec())
  {
    QString sFullName = m_ui->txtName->text() + " " + m_ui->txtSurname->text();
    qDebug() << "Error while saving instructor: " << query.lastError();
    QMessageBox::warning(this, tr("Error"), tr("Error while saving instructor %1!").arg(sFullName));
    return;
  }

  m_sSelectedId = sId;

  QModelIndex index = m_ui->tblInstructors->currentIndex();
  m_model->select();
  setDirty(false);
  m_ui->tblInstructors->setCurrentIndex(index);
}

void WInstructors::add(void)
{
  m_model->select();

  m_bEdited = false;
  m_sSelectedId = "";

  m_ui->btnDelete->setEnabled(false);
  m_ui->txtName->setText("");
  m_ui->txtSurname->setText("");
  m_ui->cbLicenceType->setCurrentIndex(0);
  m_ui->txtLicenceNumber->setText("");

  setDirty(false);
}

void WInstructors::remove(void)
{
  if (m_sSelectedId == "")
  {
    return;
  }

  QString sFullName = m_ui->txtName->text() + " " + m_ui->txtSurname->text();

  if (QMessageBox::critical(this, tr("Delete Instructor"),
                            tr("Are you sure you wan't to delete instructor %1?").arg(sFullName),
                            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
  {
    return;
  }

  QSqlQuery query;
  QString sql = "DELETE FROM instructors WHERE id=:id";

  query.prepare(sql);
  query.bindValue(":id", m_sSelectedId);
  if (!query.exec())
  {
    qDebug() << "Error while removing instructor: " << query.lastError();
    QMessageBox::warning(this, tr("Error"), tr("Error while removing instructor %1!").arg(sFullName));
    return;
  }

  add();
}

void WInstructors::setDirty(bool bDirty)
{
  if (bDirty)
  {
    m_proxy->setDirtyRow(m_ui->tblInstructors->currentIndex().row());
  }
  else
  {
    m_proxy->setDirtyRow(-1);
  }

  m_bEdited = bDirty;
  emit dirtyChanged(bDirty);
}
