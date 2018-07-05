#ifndef DB_H
#define DB_H

#include <QSqlError>
#include <QSqlQuery>

#include "job/dbmanager.h"

extern DBManager *db;

void initDb();
void uninitDb();

void createDb();

QString collate(const QString &str);
QString collateFulltextQuery(const QString &str);

#endif // DB_H
