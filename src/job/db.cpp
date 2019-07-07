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
#include "util/scopeexit.h"

DatabaseManager *db = nullptr;

DatabaseManager::DatabaseManager()
{
	auto db_ = db;
	db = this;
	SCOPE_EXIT(db = db_);

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

	if(version != CURRENT_DB_VERSION) {
		QFile(dbFilepath).copy(QString("%1.bkp.v%2").arg(dbFilepath).arg(version));
	}

#define F(v)\
	if(version == v) {\
		beginTransaction();\
		migrateDbFrom_v ## v();\
		exec("UPDATE keyValueAssoc SET value = ? WHERE key = 'database.version'", {v+1});\
		commitTransaction();\
		version++;\
	}
	DB_VERSION_HISTORY_FACTORY(F)
#undef F

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

void DatabaseManager::onSongChanged()
{
	if(!blockListChangedSignals_)
		emit sigSongListChanged();
}

QString collate(const QString &str)
{
	// \P{M} -- a character intended to be combined with another character (e.g. accents, umlauts, enclosing boxes, etc.)
	static QRegularExpression removeRegex("\\p{M}+", QRegularExpression::UseUnicodePropertiesOption);

	// Replace everything except letters and numbers with space
	static QRegularExpression clearRegex("[^a-zA-Z0-9 ]+");

	// Compact spaces
	static QRegularExpression compactSpacesRegex("\\s+");

	QString result = str.normalized(QString::NormalizationForm_KD);

	result.remove(removeRegex);
	result.replace(clearRegex, " ");
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

	result.replace(' ', "* ");
	result.append('*');

	//result.replace(' ', " NEAR ");

	return result;
}

QString denullifyString(const QString &str)
{
	return str.isNull() ? "" : str;
}
