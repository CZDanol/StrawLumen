#ifndef BIBLEREF_H
#define BIBLEREF_H

#include <QString>

class BibleRef
{

public:
	BibleRef(QString translationId, int bookId, int chapter, int verse);

public:
	QString translationId;
	int bookId, chapter, verse;

public:
	QString toString() const;

};

#endif // BIBLEREF_H
