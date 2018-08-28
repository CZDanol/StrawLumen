#ifndef EXPORTDATABASEMANAGER_H
#define EXPORTDATABASEMANAGER_H

#include "job/dbmanager.h"

#define EXPORT_DB_VERSION_HISTORY_FACTORY(F) F(1) F(2)
#define CURRENT_EXPORT_DB_VERSION 3

class ExportDatabaseManager : public DBManager
{

public:
	ExportDatabaseManager(const QString &filename, bool overwrite);

private:
	void createDb();

private:
#define F(version) void migrateExportDbFrom_v ## version(DBManager *db);
EXPORT_DB_VERSION_HISTORY_FACTORY(F)
#undef F

};

#endif // EXPORTDATABASEMANAGER_H
