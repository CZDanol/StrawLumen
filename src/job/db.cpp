#include "db.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QRegularExpression>

#include "util/standarddialogs.h"
#include "job/dbmigration.h"

DBManager *db = nullptr;

void createDb();

void criticalBootError(const QString &message) {
	standardErrorDialog(message, nullptr);
	qApp->exec();
	exit(-1);
}

void initDb()
{
	db = new DBManager();

	const QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	const QDir dbDir(dbPath);

	if( !dbDir.mkpath(".") )
		criticalBootError(DBManager::tr("Nepodařilo se vytvořit složku pro databázi: \"%1\"").arg(dbPath));

	const QString dbFilepath = dbDir.absoluteFilePath("db.sqlite");
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
						 "slideOrder TEXT"
						 ")");

		db->exec("CREATE INDEX i_songs_uid ON songs (uid)");
		db->exec("CREATE INDEX i_songs_name ON songs (name)");
		db->exec("CREATE INDEX i_songs_author_name ON songs (author, name)");
	}

	// STYLES
	{
		db->exec("CREATE TABLE styles("
						 "id INTEGER PRIMARY KEY,"
						 "name STRING,"
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

		db->exec("INSERT INTO keyValueAssoc(key, value) VALUES('database.version', 1)");
	}

	// SONGS_FULLTEXT
	{
		db->exec("CREATE VIRTUAL TABLE songs_fulltext USING fts4 ("
						 "name TEXT,"
						 "author TEXT,"
						 "content TEXT"
						 ")");
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
