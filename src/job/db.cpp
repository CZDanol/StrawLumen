#include "db.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>
#include <QUuid>

#include "main.h"
#include "util/standarddialogs.h"
#include "job/dbmigration.h"
#include "rec/songsection.h"
#include "rec/chord.h"

DatabaseManager *db = nullptr;

DatabaseManager::DatabaseManager()
{
	connect(this, SIGNAL(sigSongChanged(qlonglong)), this, SLOT(onSongChanged()));
	connect(this, SIGNAL(sigStyleChanged(qlonglong)), this, SIGNAL(sigStyleListChanged()));

	const QString dbFilepath = appDataDirectory.absoluteFilePath("db.sqlite");
	bool dbExists = QFileInfo(dbFilepath).exists();

	// Open the database
	{
		connect(this, &DBManager::sigDatabaseError, [](QString error){
			criticalBootError(DBManager::tr("Nepodařilo se incializovat databázi: %1").arg(error));
		});

		openSQLITE(dbFilepath);
		disconnect(SIGNAL(sigQueryError(QString,QString)));
	}

	connect(this, &DBManager::sigQueryError, [](QString query, QString error){
		criticalBootError(DBManager::tr("Chyba při inicializaci databáze: %1\n\n%2").arg(error, query));
	});

	if(!dbExists)
		createDb();

	int version = selectValue("SELECT value FROM keyValueAssoc WHERE key = 'database.version'").toInt();
	if(version != CURRENT_DB_VERSION)
		criticalBootError(DBManager::tr("Nepodporovaná verze databáze"));

	disconnect(SIGNAL(sigQueryError(QString,QString)));
}

DatabaseManager::~DatabaseManager()
{

}

bool DatabaseManager::blockListChangedSignals(bool set)
{
	bool result = blockListChangedSignals_;
	blockListChangedSignals_ = set;
	return result;
}

void DatabaseManager::updateSongFulltextIndex(qlonglong songId)
{
	db->exec("DELETE FROM songs_fulltext WHERE docid = ?", {songId});

	QSqlRecord r = db->selectRow("SELECT name, author, content FROM songs WHERE id = ?", {songId});

	QString content = r.value("content").toString();
	content.remove(songChordAnnotationRegex());
	content.remove(songSectionAnnotationRegex());
	content = collate(content);

	db->exec(
				"INSERT INTO songs_fulltext(docid, name, author, content) VALUES(?, ?, ?, ?)",
				{songId, collate(r.value("name").toString()), collate(r.value("author").toString()), content});
}

void DatabaseManager::createDb()
{
	// #: SONGS_TABLE_FIELDS

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

		exec("CREATE INDEX i_songs_uid ON songs (uid)");
		exec("CREATE INDEX i_songs_name ON songs (name)");
		exec("CREATE INDEX i_songs_author_name ON songs (author, name)");
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
		exec("CREATE INDEX i_song_tags_tag ON song_tags (tag)");
	}

	// STYLES
	{
		exec("CREATE TABLE styles ("
						 "id INTEGER PRIMARY KEY,"
						 "name STRING NOT NULL,"
						 "isInternal bool NOT NULL,"
						 "data BLOB"
						 ")");

		exec("CREATE INDEX i_styles_name ON styles (name)");
	}

	// BACKGROUNDS
	{
		exec("CREATE TABLE backgrounds ("
						 "id INTEGER PRIMARY KEY,"
						 "thumbnail BLOB NOT NULL,"
						 "data BLOB NOT NULL"
						 ")");
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

	if(false) {
		beginTransaction();

		QSqlQuery q(database());
		QSqlQuery q2(database());

		q.prepare("INSERT INTO songs(uid, name, author, content, slideOrder) VALUES(?, ?, ?, ?, 'V1 C V2')");
		q2.prepare("INSERT INTO songs_fulltext(docid, name, author, content) VALUES (?, ?, ?, ?)");

		for(int i = 0; i < 100000; i++) {

			QString name;
			name.resize(2+qrand()%8);
			for(int i = 0; i < name.size(); i++)
				name[i] = 'a' + qrand() % ('z' - 'a');

			QString author;
			author.resize(2+qrand()%8);
			for(int i = 0; i < author.size(); i++)
				author[i] = 'a' + qrand() % ('z' - 'a');

			QString content = "Hokus pokus";

			q.bindValue(0, QUuid::createUuid().toString());
			q.bindValue(1, name);
			q.bindValue(2, author);
			q.bindValue(3, content);
			q.exec();

			q2.bindValue(0, q.lastInsertId());
			q2.bindValue(1, collate(name));
			q2.bindValue(2, collate(author));
			q2.bindValue(3, collate(content));
			q2.exec();
		}

		commitTransaction();
	}
}

void DatabaseManager::onSongChanged()
{
	if(!blockListChangedSignals_)
		emit sigSongListChanged();
}

QString collate(const QString &str)
{
	static QRegularExpression clearRegex("[^a-zA-Z0-9 ]+");
	static QRegularExpression compactSpacesRegex("\\s+");

	QString result = str.normalized(QString::NormalizationForm_KD);

	result.remove(clearRegex);
	result.replace(compactSpacesRegex, " ");

	result = result.trimmed();
	result = result.toLower();

	return result;
}

QString collateFulltextQuery(const QString &str)
{
	QString result = collate(str);

	if(result.isEmpty())
		return result;

	result.replace(' ', " *");
	result.append('*');

	return result;
}
