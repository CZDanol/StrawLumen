#include "bibleref.h"

#include <QRegularExpression>

#include "biblebook.h"
#include "job/settings.h"
#include "job/db.h"

BibleRef::BibleRef(QString translationId, int bookId, int chapter, int verse)
{
	this->translationId = translationId;
	this->bookId = bookId;
	this->chapter = chapter;
	verses_ += verse;
	isValid_ = true;
}

BibleRef::BibleRef(QString translationId, int bookId, int chapter, const QVector<int> &verses)
{
	Q_ASSERT(!verses.isEmpty());

	this->translationId = translationId;
	this->bookId = bookId;
	this->chapter = chapter;

	verses_.append(verses);
	isValid_ = !verses_.isEmpty();
	qSort(verses_);
}

BibleRef::BibleRef(const QString &str)
{
	static const QRegularExpression generalRegex(
				"^\\s*"
				"((?:\\p{L}|[0-9.])+ *)" // book
				"\\s+"
				"([0-9]+)" // chapter
				"\\s*[:;,]\\s*" //separator
				"("
					"(?:[0-9]+(?:\\s*-\\s*[0-9]+)?)" // verse/verse range
					"(?:\\s*,\\s*[0-9]+(?:\\s*-\\s*[0-9]+)?)*" // more verse ranges
				")"
				"\\s*"
				"(\\p{L}+)?" // translation
				"\\s*$",
				QRegularExpression::CaseInsensitiveOption);

	isValid_ = false;

	QRegularExpressionMatch m = generalRegex.match(str);
	if(!m.hasMatch())
		return;

	translationId = m.captured(4);
	if(translationId.isEmpty())
		translationId = settings->defaultBibleTranslation();

	bookId = -1;
	QString bookName = collate(m.captured(1));

	for(const BibleBook &book : bibleBookList()) {
		if(book.ids.contains(bookName)) {
			bookId = book.numId;
			break;
		}
	}

	if(bookId == -1)
		return;

	chapter = m.captured(2).toInt();

	for(QString verse : m.captured(3).split(',')) {
		int delimiterPos = verse.indexOf('-');

		if(delimiterPos == -1)
			verses_ += verse.trimmed().toInt();

		else {
			int verseStart = verse.left(delimiterPos).trimmed().toInt();
			int verseEnd = verse.mid(delimiterPos+1).trimmed().toInt();
			if(verseEnd - verseStart > 1000)
				return;

			for(int i = verseStart; i <= verseEnd; i++)
				verses_ += i;
		}
	}

	isValid_ = true;
}

const QVector<int> &BibleRef::verses() const
{
	return verses_;
}

bool BibleRef::isValid() const
{
	return isValid_;
}

QString BibleRef::versesString() const
{
	if(!isValid_)
		return nullptr;

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
	if(!isValid_)
		return nullptr;

	QString result = QString("%1 %2:%3").arg(getBibleBook(bookId).id, QString::number(chapter), versesString());
	if(translationId != settings->defaultBibleTranslation())
		result += QString(" %1").arg(translationId);

	return result;
}

QString BibleRef::contentString() const
{
	if(!isValid_)
		return nullptr;

	QString result;

	QString versesStr;
	for(int v : verses_) {
		if(!versesStr.isEmpty())
			versesStr += ",";

		versesStr += QString::number(v);
	}

	QSqlQuery q = db->selectQuery(
				QString("SELECT text FROM bible_translation_verses WHERE (translation_id = ?) AND (book_id = ?) AND (chapter = ?) AND (verse IN (%1))").arg(versesStr),
				{translationId, bookId, chapter}
				);
	while(q.next()) {
		if(!result.isEmpty())
			result += ' ';

		result += q.value(0).toString();
	}

	return result;
}
