#ifndef DBMIGRATION_H
#define DBMIGRATION_H

#define DB_VERSION_HISTORY_FACTORY(F) F(1) F(2)
#define CURRENT_DB_VERSION 3

class DatabaseManager;

void createDb(DatabaseManager *db);

#define F(version) void migrateDbFrom_v ## version(DatabaseManager *db);
DB_VERSION_HISTORY_FACTORY(F)
#undef F

#endif // DBMIGRATION_H
