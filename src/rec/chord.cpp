#include "chord.h"

#include <QTextDocument>

#include "job/wordsplit.h"

Chord::Chord()
{
	isValid_ = false;
}

Chord::Chord(const QString &str)
{
	static const QRegularExpression chordRegex(
				"^"
				"([Bb][b♭]|[a-hA-H])(s|S|is|IS|es|ES|#|b|♭)?([dD]ur|m|mi|min|moll|M|maj|aug|dim|\\+)?" // Base
				"((?:[0-9(#+\\-]|sus)[0-9a-zA-Z()#+\\-]*)?" // Extra
				"(?:/([a-hA-H])(s|S|is|IS|es|ES|#|b)?)?" // Inversions
				"$",
				QRegularExpression::UseUnicodePropertiesOption);

	enum MatchPart {
		mpWhole,
		mpBaseNote, mpBaseNoteModifier,
		mpQuality, mpExtra,
		mpInversionNote, mpInversionNoteModifier
	};

	static const QHash<QString, int> notePitches {
		{"c", 0}, {"d", 2}, {"e", 4}, {"f", 5}, {"g", 7}, {"a", 9}, {"b", 10}, {"b♭", 10}, {"h", 11}
	};

	static const QHash<QString, int> noteModifierEffects {
		{"s", -1}, {"S", -1}, {"es", -1}, {"ES", -1}, {"b", -1}, {"♭", -1},
		{"is", 1}, {"IS", 1}, {"#", 1}
	};

	static const QHash<QString, Quality> qualities {
		{"dur", cvDur}, {"Dur", cvDur},
		{"m", cvMoll}, {"mi", cvMoll}, {"min", cvMoll}, {"moll", cvMoll},
		{"M", cvMaj}, {"maj", cvMaj},
		{"aug", cvAug},
		{"dim", cvDim}
	};

	QRegularExpressionMatch m = chordRegex.match(str);

	isValid_ = m.hasMatch();
	if(!isValid_)
		return;

	isFlat_ = false;

	// Base note
	QString baseNote = m.captured(mpBaseNote);

	if(baseNote == baseNote.toLower()) {
		quality_ = cvMoll;
		baseNote_ = notePitches[baseNote];
	} else {
		quality_ = cvDur;
		baseNote_ = notePitches[baseNote.toLower()];
	}

	// Base note modifier
	if(m.capturedLength(mpBaseNoteModifier)) {
		const int effect = noteModifierEffects[m.captured(mpBaseNoteModifier)];
		baseNote_ += effect;
		if(effect < 0)
			isFlat_ = true;
	}

	if(baseNote_ < 0)
		baseNote_ += 12;
	else if(baseNote_ >= 12)
		baseNote_ -= 12;

	// Base variant
	if(m.capturedLength(mpQuality))
		quality_ = qualities[m.captured(mpQuality)];

	// Added
	extra_ = m.captured(mpExtra);

	// Inversion
	if(m.capturedLength(mpInversionNote))
		inversionNote_ = notePitches[m.captured(mpInversionNote)[0].toLower()];
	else
		inversionNote_ = -1;

	if(m.capturedLength(mpInversionNoteModifier))
		inversionNote_ += noteModifierEffects[m.captured(mpInversionNoteModifier)];
}

bool Chord::isValid() const
{
	return isValid_;
}

bool Chord::isFlat() const
{
	return isFlat_;
}

Chord Chord::transposed(int by) const
{
	Chord result(*this);

	auto &base = result.baseNote_;

	base += by;
	if(base >= 12)
		base = base % 12;
	else if(base < 0)
		base = 12 - ( -base % 12 );

	auto &inv = result.inversionNote_;
	if(inv != -1) {
		inv += by;
		if(inv >= 12)
			inv = inv % 12;
		else if(inv < 0)
			inv = 12 - ( -inv % 12 );
	}

	return result;
}

QString Chord::toString(bool flatVariant) const
{
	static const QString variantStrings[_cvCount] = {"", "m", "maj", "aug", "dim"};

	static const QString noteNames[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "B♭", "H"};
	static const QString flatNoteNames[12] = {"C", "Des", "D", "Es", "E", "F", "Ges", "G", "As", "A", "B♭", "H"};

	const auto &names = flatVariant ? flatNoteNames : noteNames;

	if(!isValid_)
		return "###";

	QString result = names[baseNote_] + variantStrings[quality_] + extra_;

	if(inversionNote_ != -1) {
		result += "/";
		result += names[inversionNote_];
	}

	return result;
}

ChordsInSong songChords(const QString &song)
{
	QVector<ChordInSong> result;
	QRegularExpressionMatchIterator it = songChordAnnotationRegex().globalMatch(song);

	while(it.hasNext()) {
		QRegularExpressionMatch m = it.next();
		const Chord ch(m.captured(2));

		if(!ch.isValid())
			continue;

		result.append(ChordInSong{ch, m.capturedStart(), m.capturedLength()});
	}

	return result;
}

void transposeSong(QString &song, int by, bool flatChords)
{
	int posCorrection = 0;

	for(ChordInSong &chs : songChords(song)) {
		QString newChord = QString("[%1]").arg(chs.chord.transposed(by).toString(flatChords));

		song.replace(chs.annotationPos + posCorrection, chs.annotationLength, newChord);
		posCorrection += newChord.length() - chs.annotationLength;
	}
}

const QRegularExpression &songChordAnnotationRegex()
{
	static const QRegularExpression result("(\\[)([a-zA-Z0-9()\\-#♭/+]+)(\\])");
	return result;
}

QString removeSongChords(const QString &song)
{
	QVector<ChordInSong> chords;
	return removeSongChords(song, chords);
}

QString removeSongChords(const QString &song, QVector<ChordInSong> &chords)
{
	chords = songChords(song);

	QString result = song;
	int correction = 0;
	for(const ChordInSong &chs : chords) {
		result.remove(chs.annotationPos + correction, chs.annotationLength);
		correction -= chs.annotationLength;
	}

	return result;
}

QString copySongChords(const QString &source, const QString &target)
{
	const QStringList sourceLines = source.split('\n');
	QStringList resultLines = target.split('\n');

	ChordsInSong sourceChords;
	ChordsInSong targetChords;

	for(int lineI = 0; lineI < qMin(sourceLines.length(), resultLines.length()); lineI++) {
		const QString sourceLine = sourceLines[lineI];
		QString &resultLine = resultLines[lineI];

		QVector<int> sourceSplitPositions = WordSplit::czech(sourceLine, sourceChords, WordSplit::IncludeNewlines);
		QVector<int> targetSplitPositions = WordSplit::czech(resultLine, targetChords, WordSplit::IncludeNewlines);

		sourceSplitPositions += sourceLine.length();
		targetSplitPositions += resultLine.length();

		const int splitCount = qMin(sourceSplitPositions.length(), targetSplitPositions.length());

		int splitId = 0;
		int splitPos = 0;
		int correction = 0;
		for(const ChordInSong &chs : sourceChords) {
			while(splitId < splitCount && chs.annotationPos + chs.annotationLength >= sourceSplitPositions[splitId]) {
				splitPos = targetSplitPositions[splitId];
				splitId ++;
			}

			const bool chordIsFollowedBySpace = sourceLine.mid(chs.annotationPos + chs.annotationLength, 1) == " ";
			const QString chordAnnotation = sourceLine.mid(chs.annotationPos, chs.annotationLength); // Use exactly the same chord annotation

			resultLine.insert(splitPos + correction, chordAnnotation);
			correction += chordAnnotation.length();

			if(chordIsFollowedBySpace) {
				resultLine.insert(splitPos + correction, ' ');
				correction += 1;
			}
		}
	}

	return resultLines.join('\n');
}

ChordInSong chordAroundPos(const QString &song, int pos)
{
	int annotationStart = song.left(pos+1).lastIndexOf('[');
	if(annotationStart == -1)
		return ChordInSong();

	auto m = songChordAnnotationRegex().match(song.mid(annotationStart));
	if(!m.hasMatch() || m.capturedStart() != 0 || pos < m.capturedStart() || pos > annotationStart + m.capturedEnd())
		return ChordInSong();

	return ChordInSong{Chord(m.captured(2)), annotationStart, m.capturedLength()};
}

QVector<ChordInSong> chordsInsideSelection(const QString &song, int selectionStart, int selectionEnd)
{
	auto chords = songChords(song);
	ChordsInSong result;

	for(const ChordInSong &chs : chords) {
		if(chs.annotationPos + chs.annotationLength <= selectionStart)
			continue;

		if(chs.annotationPos >= selectionEnd)
			break;

		result += chs;
	}

	return result;
}

ChordsInSong chordsInsideSelection(const QTextCursor &textCursor)
{
	return chordsInsideSelection(textCursor.document()->toPlainText(), textCursor.selectionStart(), textCursor.selectionEnd());
}
