#include "dbmigration.h"
#include "db.h"

#define DB_MIGRATION_PROCEDURE(fromVersion, toVersion) void migrateDbFrom_v ## fromVersion(DatabaseManager *db)

void createDb(DatabaseManager *db)
{
	// #: SONGS_TABLE_FIELDS
	// Columns are set to NOT NULL so it throws errors when someone forgets to set a column value

	// SONGS
	{
		db->exec("CREATE TABLE songs ("
				 "id INTEGER PRIMARY KEY,"
				 "uid TEXT NOT NULL,"
				 "name TEXT NOT NULL,"
				 "author TEXT NOT NULL,"
				 "copyright TEXT NOT NULL,"
				 "content TEXT NOT NULL,"
				 "slideOrder TEXT NOT NULL,"
				 "lastEdit INTEGER NOT NULL"
				 ")");

		db->exec("CREATE INDEX i_songs_uid ON songs (uid)");
		db->exec("CREATE INDEX i_songs_name ON songs (name)");
		db->exec("CREATE INDEX i_songs_author_name ON songs (author, name)");
	}

	// SONGS_FULLTEXT
	{
		db->exec("CREATE VIRTUAL TABLE songs_fulltext USING fts4 ("
				 "name TEXT NOT NULL,"
				 "author TEXT NOT NULL,"
				 "content TEXT NOT NULL"
				 ")");
	}

	// SONG_TAGS
	{
		db->exec("CREATE TABLE song_tags ("
				 "song INTEGER NOT NULL,"
				 "tag TEXT NOT NULL,"
				 "PRIMARY KEY(song, tag)"
				 ")");

		db->exec("CREATE INDEX i_song_tags_song ON song_tags (song, tag)");
		db->exec("CREATE INDEX i_song_tags_tag ON song_tags (tag)");
	}

	// STYLES
	{
		db->exec("CREATE TABLE styles ("
				 "id INTEGER PRIMARY KEY,"
				 "name STRING NOT NULL,"
				 "isInternal bool NOT NULL,"
				 "data BLOB"
				 ")");

		db->exec("CREATE INDEX i_styles_name ON styles (name)");
	}

	// BACKGROUNDS
	{
		db->exec("CREATE TABLE backgrounds ("
				 "id INTEGER PRIMARY KEY,"
				 "thumbnail BLOB NOT NULL,"
				 "data BLOB NOT NULL"
				 ")");
	}

	// KEYVALUE ASSOC
	{
		db->exec("CREATE TABLE keyValueAssoc ("
				 "key STRING NOT NULL,"
				 "value"
				 ")");

		db->exec("CREATE UNIQUE INDEX i_keyValueAssoc_key ON keyValueAssoc(key)");

		db->exec("INSERT INTO keyValueAssoc(key, value)"
						 "VALUES"
						 "('database.version', 1)");
	}

	// PLAYLISTS
	{
		db->exec("CREATE TABLE playlists ("
				 "id INTEGER PRIMARY KEY,"
				 "name STRING NOT NULL,"
				 "data BLOB,"
				 "lastTouch INTEGER NOT NULL"
					")");

		db->exec("CREATE INDEX i_playlists_name ON playlists(name)");
	}
}

DB_MIGRATION_PROCEDURE(1, 2)
{
	db->exec("CREATE TABLE playlists_tmp ("
			 "id INTEGER PRIMARY KEY,"
			 "name STRING NOT NULL,"
			 "data BLOB,"
			 "lastTouch INTEGER NOT NULL"
				")");

	db->exec("INSERT INTO playlists_tmp SELECT *, 0 AS lastTouch FROM playlists");
	db->exec("DROP TABLE playlists");
	db->exec("ALTER TABLE playlists_tmp RENAME TO playlists");
}
