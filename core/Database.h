#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <QString>
#include <QUuid>
#include <QtSql>

void setupDatabase(void);
QSqlDatabase getDatabase(void);

inline QString generateId(void)
{
  return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

#endif // _DATABASE_H_
