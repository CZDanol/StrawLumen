#include "bibleref.h"

#include "biblebook.h"

BibleRef::BibleRef(QString translationId, int bookId, int chapter, int verse)
{
	this->translationId = translationId;
	this->bookId = bookId;
	this->chapter = chapter;
	this->verse = verse;
}

QString BibleRef::toString() const
{
	return QString("%2 %3:%4 %1").arg(translationId, getBibleBook(bookId).id, QString::number(chapter), QString::number(verse));
}
