#ifndef DBMIGRATION_H
#define DBMIGRATION_H

#define DB_VERSION_HISTORY_FACTORY(F) F(1) F(2) F(3) F(4) F(5) F(6) F(7)
#define CURRENT_DB_VERSION 8

class DatabaseManager;

void createDb();

#define F(version) void migrateDbFrom_v##version();
DB_VERSION_HISTORY_FACTORY(F)
#undef F

#endif// DBMIGRATION_H
