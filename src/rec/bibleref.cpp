#include "bibleref.h"

#include "biblebook.h"
#include "job/settings.h"

BibleRef::BibleRef(QString translationId, int bookId, int chapter, int verse)
{
	this->translationId = translationId;
	this->bookId = bookId;
	this->chapter = chapter;
	verses_ += verse;
}

BibleRef::BibleRef(QString translationId, int bookId, int chapter, const QVector<int> &verses)
{
	Q_ASSERT(!verses.isEmpty());

	this->translationId = translationId;
	this->bookId = bookId;
	this->chapter = chapter;

	verses_.append(verses);
	qSort(verses_);
}

const QVector<int> &BibleRef::verses() const
{
	return verses_;
}

QString BibleRef::versesString() const
{
	QStringList result;

	for(int i = 0; i < verses_.count(); i++) {
		const int startVerse = verses_[i];
		int endVerse = startVerse;

		while(i < verses_.count() - 1 && verses_[i+1] == endVerse + 1) {
			i++;
			endVerse++;
		}

		result += startVerse == endVerse ? QString::number(startVerse) : QString("%1-%2").arg(QString::number(startVerse), QString::number(endVerse));
	}

	return result.join(",");
}

QString BibleRef::toString() const
{
	QString result = QString("%1 %2:%3").arg(getBibleBook(bookId).id, QString::number(chapter), versesString());
	if(translationId != settings->defaultBibleTranslation())
		result += QString(" %1").arg(translationId);

	return result;
}
