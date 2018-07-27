#include "wordsplit.h"

#include <QRegularExpression>
#include <QDebug>

namespace WordSplit {

	void correctResult(QVector<int> &result, const ChordsInSong &chords) {
		int i = 0;
		int correction = 0;

		for(int &item : result) {
			item += correction;

			while(i < chords.length() && item >= chords[i].annotationPos) {
				correction += chords[i].annotationLength;
				item += chords[i].annotationLength;
				i ++;
			}
		}
	}

	QVector<int> czech(const QString &src, ChordsInSong &chords, int options)
	{
		QString str = removeSongChords(src, chords);

		QVector<int> result;

		static const QString rxsVowel = "(?:[aeiouyáéíóúýůě])";
		static const QString rxsConsonant = "(?:ch|kl|[bdfghjklmnpqrstvwxzčďňřšťž]|c(?!h))";

		static const QString rxsOnlyStartConsonant = QString(
					"(?:"
					"jsm|kr[km]|pl[nň]|srd|[sz][htk][lrř]|v[zž]d|vkl|z[dv][rl]"
					"|ch[cčlmrřv]"
					"[dhksvz]b|[jkrvz]d|[jlmsvzž]h|[fsvz]j|[lsštz]k|[bcčdfhkmpsštvz]l|[čdhjkrřsštvz]m|[čdfghkmpsšvz]n|[fhkmpsšvz]ň|[cčlsšvz]p|[bcčdfghkmpsštvz]r|[bdhkmpttvz]ř|[jklmpv]s|[pvz]š|[cčrsšvz]t|[cš]ť|[cčdhkrřsštz]v|[dlmrsvz]ž"
					")"
					);

		static const QString rxsStartConsonant = QString("(?:%1|%2)").arg(rxsOnlyStartConsonant, rxsConsonant);

		/// Syllable in the middle of a word
		static const QString rxsMiddleBase = QString(
					"(?:"
					"%2[aeo][uiy](?!či)"
					"|%2%1"
					"|%3(?=%2)"
					"|%1(?=%2|\\b)"
					")"
					).arg(rxsVowel, rxsStartConsonant, rxsOnlyStartConsonant);

		static const QString rxsMiddle = QString(
					"(?:"
					"%1(?:"
					"%2(?=n[áíěéý]\\b)"
					"|%2(?=%3)"
					"|(?:(?!%1)%2)*"
					")"
					")"
					).arg(rxsMiddleBase, rxsConsonant, rxsOnlyStartConsonant);

		/// Syllable in the end of the word
		static const QString rxsEnd = QString("(?:%1%2*)").arg(rxsMiddle, rxsConsonant);

		/// Syllable in the beginningh of the word
		static const QString rxsStart = QString("(?:"
																						"(?:[svzk]\\b\\s*)?(?:" // Possible (silent) preposition
																						"%4*?%2"
																						"|%3\\b"
																						"|%1"
																						"))").arg(rxsVowel, rxsMiddle, rxsEnd, rxsConsonant);

		/*qDebug() << "";
		qDebug() << "";
		qDebug() << "";
		qDebug().noquote() << rxsStartConsonant;
		qDebug() << "";
		qDebug().noquote() << rxsStart;
		qDebug() << "";
		qDebug().noquote() << rxsMiddle;
		qDebug() << "";
		qDebug().noquote() << rxsEnd;
		qDebug() << "";
		qDebug().noquote() << QString("\\b(%1)(%2*)(%3?)\\b").arg(rxsStart, rxsMiddle, rxsEnd);*/

		QRegularExpression rxWord(
					QString("\\b(%1)(%2*)(%3?)\\b|\\b\\p{L}+\\b%4").arg(rxsStart, rxsMiddle, rxsEnd, (options & IncludeNewlines) ? "|[ \t]*$" : ""),
					QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::MultilineOption);

		static QRegularExpression rxMiddle(
					QString("^(%1)(%1*)$").arg(rxsMiddle),
					QRegularExpression::CaseInsensitiveOption | QRegularExpression::UseUnicodePropertiesOption);

		auto it = rxWord.globalMatch(str);
		while(it.hasNext()) {
			const QRegularExpressionMatch m = it.next();

			result += m.capturedStart();

			if(m.capturedLength(2)) {
				int pos = m.capturedStart(2);
				QString str = m.captured(2);

				while(true) {
					auto m2 = rxMiddle.match(str);
					result += pos;
					pos += m2.capturedLength(1);
					str = m2.captured(2);

					if(str.isEmpty())
						break;
				}
			}

			if(m.capturedLength(3))
				result += m.capturedStart(3);
		}

		correctResult(result, chords);
		return result;
	}

	QString splitVisualization(const QString &str, const QVector<int> &splits)
	{
		QStringList list;
		int prev = 0;
		for(int split : splits) {
			list += str.mid(prev, split-prev);
			prev = split;
		}
		list += str.mid(prev);
		return list.join('|');
	}

}
