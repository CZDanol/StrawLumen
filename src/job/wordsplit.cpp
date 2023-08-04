#include "wordsplit.h"

#include <QRegularExpression>

namespace WordSplit {

void correctResult(QVector<int> &result, const ChordsInSong &chords, int options) {
	int chordI = 0;
	int correction = 0;

	for (int resultI = 0; resultI < result.length(); resultI++) {
		result[resultI] += correction;

		while (chordI < chords.length() && result[resultI] >= chords[chordI].annotationPos) {
			const ChordInSong &chs = chords[chordI];
			result[resultI] += chs.annotationLength;
			correction += chs.annotationLength;
			chordI++;

			if (result[resultI] != chs.annotationPos && (options & IncludeChords))
				result.insert(resultI++, chs.annotationPos);
		}
	}

	if (options & IncludeChords) {
		while (chordI < chords.length())
			result.append(chords[chordI++].annotationPos);
	}
}

QVector<int> czech(const QString &src, ChordsInSong &chords, int options) {
	QString str = removeSongChords(src, chords);

	// Ghetto support for cyrillic :(
	{
		static const QMap<QChar, QChar> cyrillicReplacements{
				{L'а', 'a'},
				{L'б', 'b'},
				{L'в', 'v'},
				{L'г', 'g'},
				{L'д', 'd'},
				{L'е', 'e'},
				{L'ё', 'o'},
				{L'ж', L'ž'},
				{L'э', 'e'},
				{L'и', 'i'},
				{L'й', 'j'},
				{L'к', 'k'},
				{L'л', 'l'},
				{L'м', 'm'},
				{L'н', 'n'},
				{L'о', 'o'},
				{L'п', 'p'},
				{L'р', 'r'},
				{L'с', 's'},
				{L'т', 't'},
				{L'у', 'u'},
				{L'ф', 'f'},
				{L'х', L'ž'},
				{L'ц', 'c'},
				{L'ч', L'č'},
				{L'ш', L'š'},
				{L'щ', L'š'},
				{L'ь', L'š'},
				{L'ы', 'y'},
				{L'ъ', L'ř'},
				{L'з', 'z'},
				{L'ю', 'j'},
				{L'я', 'j'},
		};

		str = str.toLower();
		const auto e = cyrillicReplacements.end();
		for (QChar &ch : str) {
			if (auto it = cyrillicReplacements.find(ch); it != e)
				ch = *it;
		}
	}

	QVector<int> result;

	static const QString rxsVowel = "(?:[aeiouyáéíóúýůě])";
	static const QString rxsConsonant = "(?:ch|kl|nn|mm|[bdfghjklmnpqrstvwxzčďňřšťž]|c(?!h))";

	static const QString rxsOnlyStartConsonant = //
			QString("(?:"
							"js[mt]|kr[km]|srd(?!%1)|stn|[sz][htk][lrř]|v[zž]d|vkl|vst|z[dv][rl]"
							"|ch[cčlmrřv]"
							"[dhksvz]b|[jkrvz]d|[jlmsvzž]h|[fsvz]j|[clsštz]k|[bcčdfhkmpsštvz]l|["
							"čdhjkrřsštvz]m|[čdfghkmpsšvz]n|[fhkmpsšvz]ň|[cčlsšvz]p|["
							"bcčdfghkmpsštvz]r|["
							"bdhkmpttvz]ř|[jklmpv]s|[pvz]š|[cčrsšvz]t|[cš]ť|[cčdhkrřsštz]v|["
							"dlmrsvz]ž"
							")")
					.arg(rxsVowel);

	static const QString rxsStartConsonant = QString("(?:%1|%2)").arg(rxsOnlyStartConsonant, rxsConsonant);

	/// Syllable in the middle of a word
	static const QString rxsMiddleBase = //
			QString("(?:"
							"%2[aeo][uiy](?!či)"
							"|%2%1"
							"|%3(?=%2)"
							"|%1(?=%2|\\b)"
							")")
					.arg(rxsVowel, rxsStartConsonant, rxsOnlyStartConsonant);

	static const QString rxsMiddle = //
			QString("(?:"
							"%1(?:"
							"%2(?=n[áíěéý]\\b)"
							"|%2(?=%3)"
							"|(?:(?!%1)%2)*"
							")"
							")")
					.arg(rxsMiddleBase, rxsConsonant, rxsOnlyStartConsonant);

	/// Syllable in the end of the word
	static const QString rxsEnd = //
			QString("(?:%1%2*)").arg(rxsMiddle, rxsConsonant);

	/// Syllable in the beginningh of the word
	static const QString rxsStart = //
			QString("(?:"
							"(?:[svzk]\\b\\s*)?(?:" // Possible (silent) preposition
							"%4*?%2"
							"|%3\\b"
							"|%1"
							"))")
					.arg(rxsVowel, rxsMiddle, rxsEnd, rxsConsonant);

	static const QRegularExpression rxWord( //
			QString("\\b(%1)(%2*)(%3?)\\b"
							"|\\b\\p{L}+\\b%4")
					.arg(rxsStart, rxsMiddle, rxsEnd, (options & IncludeNewlines) ? "|[ \t]*$" : "" //
							),
			QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::MultilineOption);

	static QRegularExpression rxMiddle(																														 //
			QString("^(%1)(%1*)$").arg(rxsMiddle),																										 //
			QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption //
	);

	auto it = rxWord.globalMatch(str);
	while (it.hasNext()) {
		const QRegularExpressionMatch m = it.next();

		result += m.capturedStart();

		if (m.capturedLength(2)) {
			int pos = m.capturedStart(2);
			QString str = m.captured(2);

			while (true) {
				auto m2 = rxMiddle.match(str);
				result += pos;
				pos += m2.capturedLength(1);
				str = m2.captured(2);

				if (str.isEmpty())
					break;
			}
		}

		if (m.capturedLength(3))
			result += m.capturedStart(3);
	}

	correctResult(result, chords, options);
	return result;
}

QString splitVisualization(const QString &str, const QVector<int> &splits) {
	QStringList list;
	int prev = 0;
	for (int split : splits) {
		list += str.mid(prev, split - prev);
		prev = split;
	}
	list += str.mid(prev);
	return list.join('|');
}

} // namespace WordSplit
