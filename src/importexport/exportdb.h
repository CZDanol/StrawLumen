#ifndef EXPORTDATABASEMANAGER_H
#define EXPORTDATABASEMANAGER_H

#include "job/dbmanager.h"

class ExportDatabaseManager : public DBManager
{

public:
	ExportDatabaseManager(const QString &filename, bool overwrite);

private:
	void createDb();

};

#endif // EXPORTDATABASEMANAGER_H
