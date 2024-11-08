#ifndef BIBLEREF_H
#define BIBLEREF_H

#include <QRegularExpression>
#include <QString>
#include <QVector>

class BibleRef {

public:
	static const QRegularExpression &regex();

	BibleRef();
	BibleRef(QString translationId, int bookId, int chapter, int verse);
	BibleRef(QString translationId, int bookId, int chapter, const QVector<int> &verses);
	BibleRef(const QStringList &translationIds, int bookId, int chapter, const QVector<int> &verses);
	BibleRef(const QString &str);

public:
	QStringList translationIds;
	int bookId, chapter;

public:
	const QVector<int> &verses() const;

public:
	bool isValid() const;

	QString versesString() const;
	QString toString() const;

	QString contentString() const;

private:
	QVector<int> verses_;
	bool isValid_;
};

#endif// BIBLEREF_H
