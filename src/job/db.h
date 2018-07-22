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

public:
	bool blockListChangedSignals(bool set);

public:
	void updateSongFulltextIndex(qlonglong songId);

private slots:
	void onSongChanged();

private:
	bool blockListChangedSignals_ = false;

};

extern DatabaseManager *db;

QString collate(const QString &str);
QString collateFulltextQuery(const QString &str);
QString denullifyString(const QString &str);

#endif // DB_H
