#ifndef BIBLEREF_H
#define BIBLEREF_H

#include <QString>
#include <QVector>

class BibleRef
{

public:
	BibleRef(QString translationId, int bookId, int chapter, int verse);
	BibleRef(QString translationId, int bookId, int chapter, const QVector<int> &verses);

public:
	QString translationId;
	int bookId, chapter;

public:
	const QVector<int> &verses() const;

public:
	QString versesString() const;
	QString toString() const;

private:
	QVector<int> verses_;

};

#endif // BIBLEREF_H
