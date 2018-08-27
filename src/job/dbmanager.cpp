#include "dbmanager.h"

#include <QMutexLocker>
#include <QSqlError>
#include <QSqlQuery>
#include <QPointer>
#include <QHash>
#include <QVector>

DBManager::DBManager()
{

}

DBManager::~DBManager()
{
	if(db_.isOpen())
		db_.close();

	QSqlDatabase::removeDatabase(db_.connectionName());
}

void DBManager::openSQLITE(const QString &filename)
{
	QByteArray uniqIdBytes;
	DBManager *thisPtr = this;
	uniqIdBytes.append(reinterpret_cast<const char*>(&thisPtr), sizeof(thisPtr));
	QString uniqId = QString::fromLatin1(uniqIdBytes.toHex());

	db_ = QSqlDatabase::addDatabase("QSQLITE", uniqId);
	db_.setDatabaseName(filename);

	if(!db_.open())
		emit sigDatabaseError(db_.lastError().text());
}

QSqlDatabase &DBManager::database()
{
	return db_;
}

void DBManager::execAssoc(QString query, const AssocArgs &args)
{
	selectQueryAssoc(query, args);
}

void DBManager::exec(QString query, const DBManager::Args &args)
{
	selectQuery(query, args);
}

QVariant DBManager::insertAssoc(const QString &query, const DBManager::AssocArgs &args)
{
	return selectQueryAssoc(query, args).lastInsertId();
}

QVariant DBManager::insert(const QString &query, const DBManager::Args &args)
{
	return selectQuery(query, args).lastInsertId();
}

QVariant DBManager::insert(const QString &table, const QHash<QString, QVariant> &fields)
{
	return insert(QString("INSERT INTO %1(%2) VALUES(%3)").arg(table, QStringList(fields.keys()).join(","), QStringList(QVector<QString>(fields.size(), "?").toList()).join(",")), fields.values());
}

void DBManager::update(const QString &table, const QHash<QString, QVariant> &fields, const QString &where, const QVariantList &whereArgs)
{
	QStringList setList;
	setList.reserve(fields.size());
	for(const QString &field : fields.keys())
		setList.append(QString("%1 = ?").arg(field));

	exec(QString("UPDATE %1 SET %2 WHERE %3").arg(table, setList.join(","), where), QVariantList() << fields.values() << whereArgs);
}

QSqlRecord DBManager::selectRowAssoc(const QString &query, const DBManager::AssocArgs &args)
{
	QSqlQuery q = selectQueryAssoc(query, args);

	if(!q.next())
		emit sigQueryError(queryDesc(q, args), "No rows returned (selectRow)");

	return q.record();
}

QSqlRecord DBManager::selectRow(const QString &query, const DBManager::Args &args)
{
	QSqlQuery q = selectQuery(query, args);

	if(!q.next())
		emit sigQueryError(queryDesc(q, args), "No rows returned (selectRow)");

	return q.record();
}

QSqlRecord DBManager::selectRowDefAssoc(const QString &query, const DBManager::AssocArgs &args, QSqlRecord def)
{
	QSqlQuery q = selectQueryAssoc(query, args);

	if(!q.next())
		return def;

	return q.record();
}

QSqlRecord DBManager::selectRowDef(const QString &query, const DBManager::Args &args, QSqlRecord def)
{
	QSqlQuery q = selectQuery(query, args);

	if(!q.next())
		return def;

	return q.record();
}

QVariant DBManager::selectValueAssoc(const QString &query, const DBManager::AssocArgs &args)
{
	QSqlQuery q = selectQueryAssoc(query, args);

	if(!q.next())
		emit sigQueryError(queryDesc(q, args), "No rows returned (selectValue)");

	return q.value(0);
}

QVariant DBManager::selectValue(const QString &query, const DBManager::Args &args)
{
	QSqlQuery q = selectQuery(query, args);

	if(!q.next())
		emit sigQueryError(queryDesc(q, args), "No rows returned (selectValue)");

	return q.value(0);
}

QVariant DBManager::selectValueDefAssoc(const QString &query, const DBManager::AssocArgs &args, const QVariant &def)
{
	QSqlQuery q = selectQueryAssoc(query, args);

	if(!q.next())
		return def;

	return q.value(0);
}

QVariant DBManager::selectValueDef(const QString &query, const DBManager::Args &args, const QVariant &def)
{
	QSqlQuery q = selectQuery(query, args);

	if(!q.next())
		return def;

	return q.value(0);
}

QSqlQuery DBManager::selectQueryAssoc(const QString &query, const DBManager::AssocArgs &args)
{
	QSqlQuery q(db_);
	if(!q.prepare(query)) {
		emit sigQueryError(query, q.lastError().databaseText());
		return q;
	}

	for(AssocArg arg : args)
		q.bindValue(arg.first, arg.second);

	if(!q.exec())
		emit sigQueryError(query, q.lastError().databaseText());

	return q;
}

QSqlQuery DBManager::selectQuery(const QString &query, const DBManager::Args &args)
{
	QSqlQuery q(db_);
	if(!q.prepare(query)) {
		emit sigQueryError(query, q.lastError().databaseText());
		return q;
	}

	for(int i = 0; i < args.length(); i ++)
		q.bindValue(i, args[i]);

	if(!q.exec())
		emit sigQueryError(query, q.lastError().databaseText());

	return q;
}

bool DBManager::beginTransaction()
{
	return db_.transaction();
}

bool DBManager::commitTransaction()
{
	return db_.commit();
}

QString DBManager::queryDesc(const QSqlQuery &q, const Args &args)
{
	QString result = q.lastQuery() + " [";
	for(int i = 0; i < args.size(); i ++) {
		if(i)
			result += ", ";

		result += args[i].toString();
	}
	result += "]";
	return result;
}

QString DBManager::queryDesc(const QSqlQuery &q, const AssocArgs &args)
{
	QString result = q.lastQuery() + " {";
	for(int i = 0; i < args.size(); i ++) {
		if(i)
			result += ", ";

		result += args[i].first + "= " + args[i].second.toString();
	}
	result += "}";
	return result;
}
