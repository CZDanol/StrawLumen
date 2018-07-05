#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QVector>
#include <QPair>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlRecord>

class DBManager : public QObject
{
	Q_OBJECT

public:
	friend class DBQuery;

	using Arg = QVariant;
	using Args = QVector<QVariant>;

	using AssocArg = QPair<QString,QVariant>;
	using AssocArgs = QVector<AssocArg>;

	using ManipFunc = std::function<void(QSqlQuery&)>;
	using QueryOpFunc = std::function<void(QSqlDatabase&)>;

public:
	DBManager();
	~DBManager();

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

	/// Blocking query, returns first row selected
	QSqlRecord selectRowAssoc(const QString &query, const AssocArgs &args = AssocArgs());
	QSqlRecord selectRow(const QString &query, const Args &args = Args());

	/// Blocking query, returns first row selected (returns def if no rows)
	QSqlRecord selectRowDefAssoc(const QString &query, const AssocArgs &args = AssocArgs(), QSqlRecord def = QSqlRecord());
	QSqlRecord selectRowDef(const QString &query, const Args &args = Args(), QSqlRecord def = QSqlRecord());

	/// Blocking query, returns first value of the first row
	QVariant selectValueAssoc(const QString &query, const AssocArgs &args = AssocArgs());
	QVariant selectValue(const QString &query, const Args &args = Args());

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
	void sigOpenError(QString error);

private:
	QSqlDatabase db_;

};

#endif // DBMANAGER_H
