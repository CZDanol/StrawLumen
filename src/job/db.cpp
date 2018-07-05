#include "db.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>
#include <QUuid>

#include "main.h"
#include "util/standarddialogs.h"
#include "job/dbmigration.h"

DBManager *db = nullptr;

void initDb()
{
	db = new DBManager();

	const QString dbFilepath = appDataDirectory.absoluteFilePath("db.sqlite");
	bool dbExists = QFileInfo(dbFilepath).exists();

	// Open the database
	{
		QObject::connect(db, &DBManager::sigOpenError, [](QString error){
			criticalBootError(DBManager::tr("Nepodařilo se incializovat databázi: %1").arg(error));
		});

		db->openSQLITE(dbFilepath);

		db->disconnect(SIGNAL(sigQueryError(QString,QString)));
	}

	QObject::connect(db, &DBManager::sigQueryError, [](QString query, QString error){
		criticalBootError(DBManager::tr("Chyba při inicializaci databáze: %1\n\n%2").arg(error, query));
	});

	if(!dbExists)
		createDb();

	int version = db->selectValue("SELECT value FROM keyValueAssoc WHERE key = 'database.version'").toInt();
	if(version != CURRENT_DB_VERSION)
		criticalBootError(DBManager::tr("Nepodporovaná verze databáze"));

	db->disconnect(SIGNAL(sigQueryError(QString,QString)));
}

void uninitDb()
{
	delete db;
}

void createDb() {
	// SONGS
	{
		db->exec("CREATE TABLE songs ("
						 "id INTEGER PRIMARY KEY,"
						 "uid TEXT,"
						 "name TEXT,"
						 "author TEXT,"
						 "content TEXT,"
						 "slideOrder TEXT,"
						 "lastEdit INTEGER"
						 ")");

		db->exec("CREATE INDEX i_songs_uid ON songs (uid)");
		db->exec("CREATE INDEX i_songs_name ON songs (name)");
		db->exec("CREATE INDEX i_songs_author_name ON songs (author, name)");
	}

	// STYLES
	{
		db->exec("CREATE TABLE styles ("
						 "id INTEGER PRIMARY KEY,"
						 "name STRING,"
						 "data BLOB"
						 ")");

		db->exec("CREATE INDEX i_styles_name ON styles (name)");
	}

	// BACKGROUNDS
	{
		db->exec("CREATE TABLE backgrounds ("
						 "id INTEGER PRIMARY KEY,"
						 "thumbnail BLOB,"
						 "data BLOB"
						 ")");
	}

	// KEYVALUE ASSOC
	{
		db->exec("CREATE TABLE keyValueAssoc ("
						 "key STRING,"
						 "value"
						 ")");

		db->exec("CREATE INDEX i_keyValueAssoc_key ON keyValueAssoc(key)");

		db->exec("INSERT INTO keyValueAssoc(key, value)"
						 "VALUES"
						 "('database.version', 1)");
	}

	// SONGS_FULLTEXT
	{
		db->exec("CREATE VIRTUAL TABLE songs_fulltext USING fts4 ("
						 "name TEXT,"
						 "author TEXT,"
						 "content TEXT"
						 ")");
	}

	if(false) {
		db->beginTransaction();

		QSqlQuery q(db->database());
		QSqlQuery q2(db->database());

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

		db->commitTransaction();
	}
}

QString collate(const QString &str)
{
	static QRegularExpression clearRegex("[^a-zA-Z0-9 ]+");
	static QRegularExpression compactSpacesRegex("\\s+");

	QString result = str.normalized(QString::NormalizationForm_KD);

	result.replace(clearRegex, QString());
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

	result.replace(' ', "* *");
	result.prepend('*');
	result.append('*');

	return result;
}
