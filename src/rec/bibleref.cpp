#include "bibleref.h"

#include <QRegularExpression>

#include "biblebook.h"
#include "job/db.h"
#include "job/settings.h"

#define TRANSLATIONID_REGEX "\\p{L}(?:\\p{L}|[0-9_])*"
#define TRANSLATIONID_SEP_REGEX "[,+]"

const QRegularExpression &BibleRef::regex() {
	static const QRegularExpression r(
	  "^\\s*"
	  "((?:[0-9])*\\s*\\.?\\s*(?:\\p{L}|[ ])*)"// book
	  "\\s+"
	  "([0-9]+)"     // chapter
	  "\\s*[:;,]\\s*"//separator
	  "("
	  "(?:[0-9]+(?:\\s*-\\s*[0-9]+)?)"             // verse/verse range
	  "(?:\\s*,\\s*[0-9]+(?:\\s*[-–]\\s*[0-9]+)?)*"// more verse ranges
	  ")"
	  ",?"
	  "\\s*"
	  "(" TRANSLATIONID_REGEX "(?:\\s*" TRANSLATIONID_SEP_REGEX "\\s*" TRANSLATIONID_REGEX ")*)?"// translation
	  "\\s*$",
	  QRegularExpression::CaseInsensitiveOption);

	return r;
}

BibleRef::BibleRef() {
	isValid_ = false;
}

BibleRef::BibleRef(QString translationId, int bookId, int chapter, int verse)
    : BibleRef(QStringList{translationId}, bookId, chapter, QVector<int>{verse}) {
}

BibleRef::BibleRef(QString translationId, int bookId, int chapter, const QVector<int> &verses)
    : BibleRef(QStringList{translationId}, bookId, chapter, verses) {
}

BibleRef::BibleRef(const QStringList &translationIds, int bookId, int chapter, const QVector<int> &verses) {
	Q_ASSERT(!verses.isEmpty());

	this->translationIds = translationIds;
	this->bookId = bookId;
	this->chapter = chapter;

	verses_.append(verses);
	isValid_ = !verses_.isEmpty();
	std::sort(verses_.begin(), verses_.end());
}

BibleRef::BibleRef(const QString &str) {
	isValid_ = false;

	QRegularExpressionMatch m = regex().match(str);
	if(!m.hasMatch())
		return;

	if(m.capturedLength(4)) {
		static const QRegularExpression sepRegex(TRANSLATIONID_SEP_REGEX);
		translationIds = m.captured(4).split(sepRegex);
		for(QString &t: translationIds)
			t = t.trimmed();
	}
	else
		translationIds += settings->setting_defaultBibleTranslation();

	bookId = -1;
	QString bookName = collate(m.captured(1));
	// Remove all spaces
	bookName.remove(' ');

	for(const BibleBook &book: bibleBookList()) {
		if(book.ids.contains(bookName)) {
			bookId = book.numId;
			break;
		}
	}

	if(bookId == -1)
		return;

	chapter = m.captured(2).toInt();

	for(QString verse: m.captured(3).split(',')) {
		int delimiterPos = verse.indexOf('-');

		if(delimiterPos == -1)
			verses_ += verse.trimmed().toInt();

		else {
			int verseStart = verse.left(delimiterPos).trimmed().toInt();
			int verseEnd = verse.mid(delimiterPos + 1).trimmed().toInt();
			if(verseEnd - verseStart > 1000)
				return;

			for(int i = verseStart; i <= verseEnd; i++)
				verses_ += i;
		}
	}

	isValid_ = true;
}

const QVector<int> &BibleRef::verses() const {
	return verses_;
}

bool BibleRef::isValid() const {
	return isValid_;
}

QString BibleRef::versesString() const {
	if(!isValid_)
		return nullptr;

	QStringList result;

	for(int i = 0; i < verses_.count(); i++) {
		const int startVerse = verses_[i];
		int endVerse = startVerse;

		while(i < verses_.count() - 1 && verses_[i + 1] == endVerse + 1) {
			i++;
			endVerse++;
		}

		result += startVerse == endVerse ? QString::number(startVerse) : QString("%1-%2").arg(QString::number(startVerse), QString::number(endVerse));
	}

	return result.join(",");
}

QString BibleRef::toString() const {
	if(!isValid_)
		return nullptr;

	QString result = QString("%1 %2:%3").arg(getBibleBook(bookId).id, QString::number(chapter), versesString());
	if(translationIds != settings->setting_defaultBibleTranslation())
		result += QString(" %1").arg(translationIds.join('+'));

	return result;
}

QString BibleRef::contentString() const {
	if(!isValid_)
		return nullptr;

	QString result;

	QString versesStr;
	for(int v: verses_) {
		if(!versesStr.isEmpty())
			versesStr += ",";

		versesStr += QString::number(v);
	}

	static const QRegularExpression startsWithSmallLetterRegex("^\\s*\\p{Ll}");
	static const QRegularExpression endsWithSentenceEndRegex(".*[.!?][^.!?,;:\\-–]*?$");
	static const QRegularExpression endsWithMarkRegex(".*[,;:\\-–]$");

	for(const QString &translationId: translationIds) {
		int previousVerse = -1;
		QString previousVerseStr;

		QSqlQuery q = db->selectQuery(
		  QString("SELECT text, verse FROM bible_translation_verses WHERE (translation_id = ?) AND (book_id = ?) AND (chapter = ?) AND (verse IN (%1)) ORDER BY verse ASC").arg(versesStr),
		  {translationId, bookId, chapter});
		while(q.next()) {
			const int verse = q.value(1).toInt();
			const QString verseStr = q.value(0).toString();

			// Previous verse is not connected and does not end with . -> sentence,...
			if(previousVerse != -1 && previousVerse != verse - 1 && !previousVerseStr.contains(endsWithSentenceEndRegex)) {
				if(previousVerseStr.contains(endsWithMarkRegex))
					result += ' ';

				result += "…";
			}

			if(!result.isEmpty())
				result += ' ';

			// Previous verse is not connected
			if(previousVerse != -1 && previousVerse != verse - 1)
				result += "(…) ";

			// Verses are not connected and this verse does not start with a capital letter -> ...text
			if(previousVerse != verse - 1 && verseStr.contains(startsWithSmallLetterRegex))
				result += "…";

			result += verseStr;
			previousVerse = verse;
			previousVerseStr = verseStr;
		}

		// Last verse does not end with . -> add ...
		if(!previousVerseStr.contains(endsWithSentenceEndRegex))
			result += "…";

		result += "\n\n";
	}

	return result.trimmed();
}
