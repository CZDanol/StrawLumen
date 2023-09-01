#include "exportdb.h"

#include <QFile>
#include <QSqlQuery>

#include "gui/mainwindow.h"
#include "job/db.h"
#include "rec/chord.h"
#include "util/macroutils.h"

#define EXPORT_DB_MIGRATION_PROCEDURE(fromVersion, toVersion) void ExportDatabaseManager::migrateExportDbFrom_v ## fromVersion(DBManager *db)

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

#define F(v)\
	if(version == v) {\
		beginTransaction();\
		migrateExportDbFrom_v ## v(this);\
		exec("UPDATE keyValueAssoc SET value = ? WHERE key = 'database.version'", {v+1});\
		commitTransaction();\
		version++;\
	}
	EXPORT_DB_VERSION_HISTORY_FACTORY(F)
#undef F

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
				 "standardized_name TEXT NOT NULL,"
				 "author TEXT NOT NULL,"
				 "copyright TEXT NOT NULL,"
				 "content TEXT NOT NULL,"
				 "notes TEXT NOT NULL,"
				 "slideOrder TEXT NOT NULL,"
				 "lastEdit INTEGER NOT NULL"
				 ")");
	}

	// SONGS_FULLTEXT
	{
		exec("CREATE VIRTUAL TABLE songs_fulltext USING fts4 ("
				 "name TEXT NOT NULL,"
				 "author TEXT NOT NULL,"
				 "content TEXT NOT NULL"
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
						 "('database.version', " STRINGIFY(CURRENT_EXPORT_DB_VERSION) ")");
	}
}

EXPORT_DB_MIGRATION_PROCEDURE(1,2)
{
	db->exec("CREATE TABLE songs_tmp ("
			 "id INTEGER PRIMARY KEY,"
			 "uid TEXT NOT NULL,"
			 "name TEXT NOT NULL,"
			 "author TEXT NOT NULL,"
			 "copyright TEXT NOT NULL,"
			 "content TEXT NOT NULL,"
			 "notes TEXT NOT NULL,"
			 "slideOrder TEXT NOT NULL,"
			 "lastEdit INTEGER NOT NULL"
			 ")");

	db->exec("INSERT INTO songs_tmp SELECT id, uid, name, author, copyright, content, '' AS notes, slideOrder, lastEdit FROM songs");
	db->exec("DROP TABLE songs");
	db->exec("ALTER TABLE songs_tmp RENAME TO songs");

	db->exec("CREATE INDEX i_songs_uid ON songs (uid)");
	db->exec("CREATE INDEX i_songs_name ON songs (name)");
	db->exec("CREATE INDEX i_songs_author_name ON songs (author, name)");
}


EXPORT_DB_MIGRATION_PROCEDURE(2,3)
{
	db->exec("UPDATE songs SET uid = CAST(uid AS TEXT)");
}

EXPORT_DB_MIGRATION_PROCEDURE(3,4)
{
	db->exec("ALTER TABLE songs ADD standardized_name TEXT");

	db->beginTransaction();

	auto q = db->selectQuery("SELECT id, name FROM songs");
	while(q.next())
		db->exec("UPDATE songs SET standardized_name = ? WHERE id = ?", {standardizeSongName(q.value(1).toString()), q.value(0)});

	db->commitTransaction();
}

EXPORT_DB_MIGRATION_PROCEDURE(4, 5) {
	db->exec("CREATE VIRTUAL TABLE songs_fulltext USING fts4 ("
					 "name TEXT NOT NULL,"
					 "author TEXT NOT NULL,"
					 "content TEXT NOT NULL"
					 ")");

	QSqlQuery q = db->selectQuery("SELECT id FROM songs");
	while (q.next())
		DatabaseManager::updateSongFulltextIndex(db, q.value(0).toInt());
}
