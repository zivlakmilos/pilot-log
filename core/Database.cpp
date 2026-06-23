#include "Database.h"

#include <QCoreApplication>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>

void runMigrations(void);
void showFatalError(const QString &error);

void setupDatabase(void)
{
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

#if PRODUCTION
#else  // PRODUCTION
  db.setDatabaseName("database.db");
#endif // PRODUCTION

  if (!db.open())
  {
    QMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("Cannot Open Database"));
    qFatal() << "cannot open database";
    qApp->exit();
    showFatalError(QObject::tr("Cannot Open Database"));
  }

  runMigrations();
}

QSqlDatabase getDatabase()
{
  return QSqlDatabase::database();
}

void runMigrations(void)
{
  QSqlDatabase db = QSqlDatabase::database();
  QSqlQuery query(db);
  QString sql;

  sql = "CREATE TABLE IF NOT EXISTS pilots ("
        "  id TEXT PRIMARY KEY,"
        "  name TEXT,"
        "  surname TEXT,"
        "  licence TEXT,"
        "  licenceNnumber TEXT"
        ")";

  if (!query.exec(sql))
  {
    showFatalError("Creating table 'pilots' failed");
  }

  sql = "CREATE TABLE IF NOT EXISTS instructors ("
        "  id TEXT PRIMARY KEY,"
        "  name TEXT,"
        "  surname TEXT,"
        "  licenceType TEXT,"
        "  licenceNumber TEXT"
        ")";

  if (!query.exec(sql))
  {
    showFatalError("Creating table 'instructors' failed");
  }

  sql = "CREATE TABLE IF NOT EXISTS airplanes ("
        "  id TEXT PRIMARY KEY,"
        "  model TEXT,"
        "  registration TEXT,"
        "  airplaneClass TEXT"
        ")";

  if (!query.exec(sql))
  {
    showFatalError("Creating table 'airplanes' failed");
  }

  sql = "CREATE TABLE IF NOT EXISTS logs ("
        "  id TEXT PRIMARY KEY,"
        "  date TEXT,"
        "  departureAirport TEXT,"
        "  departureTime TEXT,"
        "  arrivalAirport TEXT,"
        "  arrivalTime TEXT,"
        "  airplaneId TEXT,"
        "  sepTime TEXT,"
        "  mepTime TEXT,"
        "  multiPilotTime TEXT,"
        "  totalFlightTime TEXT,"
        "  name TEXT,"
        "  langingsDay INTEGER,"
        "  langingsNight INTEGER,"
        "  nightTime TEXT,"
        "  ifrTime TEXT,"
        "  picTime TEXT,"
        "  copliotTime TEXT,"
        "  dualTime TEXT,"
        "  instructorTime TEXT,"
        "  simulatorDate TEXT,"
        "  simulatorType TEXT,"
        "  simulatorSessionTime TEXT,"
        "  remarks TEXT"
        ")";

  if (!query.exec(sql))
  {
    showFatalError("Creating table 'logs' failed");
  }
}

void showFatalError(const QString &error)
{
  QMessageBox::critical(nullptr, QObject::tr("Error"), error);
  qFatal() << error;
  qApp->exit();
}
