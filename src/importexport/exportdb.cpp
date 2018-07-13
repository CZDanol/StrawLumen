#include "exportdb.h"

#include <QFile>

#include "gui/mainwindow.h"

#define CURRENT_EXPORT_DB_VERSION 1

ExportDatabaseManager::ExportDatabaseManager(const QString &filename, bool overwrite)
{
	connect(this, SIGNAL(sigDatabaseError(QString)), mainWindow, SLOT(onDbDatabaseError(QString)));
	connect(this, SIGNAL(sigQueryError(QString,QString)), mainWindow, SLOT(onDbQueryError(QString,QString)));

	QFile f(filename);
	const bool fileExists = f.exists();

	if(overwrite && fileExists)
		f.remove();

	openSQLITE(filename);

	if(!fileExists || overwrite)
		createDb();

	int version = selectValue("SELECT value FROM keyValueAssoc WHERE key = 'database.version'").toInt();
	if(version != CURRENT_EXPORT_DB_VERSION)
		emit sigDatabaseError(DBManager::tr("Nepodporovaná verze databáze"));
}

void ExportDatabaseManager::createDb()
{
	// #: SONGS_TABLE_FIELDS
	// Columns are set to NOT NULL so it throws errors when someone forgets to set a column value

	// SONGS
	{
		exec("CREATE TABLE songs ("
						 "id INTEGER PRIMARY KEY,"
						 "uid TEXT NOT NULL,"
						 "name TEXT NOT NULL,"
						 "author TEXT NOT NULL,"
						 "copyright TEXT NOT NULL,"
						 "content TEXT NOT NULL,"
						 "slideOrder TEXT NOT NULL,"
						 "lastEdit INTEGER NOT NULL"
						 ")");
	}

	// SONG_TAGS
	{
		exec("CREATE TABLE song_tags ("
				 "song INTEGER NOT NULL,"
				 "tag TEXT NOT NULL"
				 ")");

		exec("CREATE INDEX i_song_tags_song ON song_tags (song, tag)");
	}

	// KEYVALUE ASSOC
	{
		exec("CREATE TABLE keyValueAssoc ("
						 "key STRING NOT NULL,"
						 "value"
						 ")");

		exec("CREATE INDEX i_keyValueAssoc_key ON keyValueAssoc(key)");

		exec("INSERT INTO keyValueAssoc(key, value)"
						 "VALUES"
						 "('database.version', 1)");
	}
}
