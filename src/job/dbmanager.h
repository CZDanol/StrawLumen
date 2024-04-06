#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QPair>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QVariant>
#include <QVector>

class DBManager : public QObject {
	Q_OBJECT

public:
	friend class DBQuery;

	using Arg = QVariant;
	using Args = QVariantList;

	using AssocArg = QPair<QString, QVariant>;
	using AssocArgs = QList<AssocArg>;

	using ManipFunc = std::function<void(QSqlQuery &)>;
	using QueryOpFunc = std::function<void(QSqlDatabase &)>;

public:
	DBManager();
	virtual ~DBManager();

public:
	void openSQLITE(const QString &filename);

	QSqlDatabase &database();

public:
	/// Nonblocking query
	void execAssoc(QString query, const AssocArgs &args = AssocArgs());
	void exec(QString query, const Args &args = Args());

	/// Blocking query, returns insert id
	QVariant insertAssoc(const QString &query, const AssocArgs &args = AssocArgs());
	QVariant insert(const QString &query, const Args &args = Args());
	QVariant insert(const QString &table, const QHash<QString, QVariant> &fields);

	void update(const QString &table, const QHash<QString, QVariant> &fields, const QString &where, const QVariantList &whereArgs = QVariantList());

	/// Blocking query, returns first row selected (or throws error if no rows)
	QSqlRecord selectRowAssoc(const QString &query, const AssocArgs &args = AssocArgs());
	QSqlRecord selectRow(const QString &query, const Args &args = Args());

	/// Blocking query, returns first row selected (returns def if no rows)
	QSqlRecord selectRowDefAssoc(const QString &query, const AssocArgs &args = AssocArgs(), QSqlRecord def = QSqlRecord());
	QSqlRecord selectRowDef(const QString &query, const Args &args = Args(), QSqlRecord def = QSqlRecord());

	/// Blocking query, returns first value of the first row (or throws error if no rows)
	QVariant selectValueAssoc(const QString &query, const AssocArgs &args = AssocArgs());
	QVariant selectValue(const QString &query, const Args &args = Args());

	/// Blocking query, returns first value of the first row
	QVariant selectValueDefAssoc(const QString &query, const AssocArgs &args = AssocArgs(), const QVariant &def = QVariant());
	QVariant selectValueDef(const QString &query, const Args &args = Args(), const QVariant &def = QVariant());

	/// Blocking query, returns the query
	QSqlQuery selectQueryAssoc(const QString &query, const AssocArgs &args = AssocArgs());
	QSqlQuery selectQuery(const QString &query, const Args &args = Args());

public:
	bool beginTransaction();
	bool commitTransaction();

public:
	QString queryDesc(const QSqlQuery &q, const Args &args);
	QString queryDesc(const QSqlQuery &q, const AssocArgs &args);

signals:
	void sigQueryError(QString query, QString error);
	void sigDatabaseError(QString error);

private:
	QSqlDatabase db_;
};

#endif// DBMANAGER_H
