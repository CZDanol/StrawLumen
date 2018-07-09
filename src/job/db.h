#ifndef DB_H
#define DB_H

#include <QSqlError>
#include <QSqlQuery>

#include "job/dbmanager.h"

class DatabaseManager : public DBManager
{
	Q_OBJECT

public:
	DatabaseManager();
	~DatabaseManager();

signals:
	void sigSongListChanged();
	void sigStyleListChanged();

	// Following signals automatically emit adequate sitXXXListChanged
	void sigSongChanged(qlonglong songId);
	void sigStyleChanged(qlonglong styleId);

private:
	void createDb();

};

extern DatabaseManager *db;

QString collate(const QString &str);
QString collateFulltextQuery(const QString &str);

#endif // DB_H
