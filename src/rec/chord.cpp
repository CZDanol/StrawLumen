#include "chord.h"

#include "gui/settingsdialog.h"

Chord::Chord()
{
	isValid_ = false;
}

Chord::Chord(const QString &str)
{
	static const QRegularExpression chordRegex(
				"^"
				"([a-hA-H])(s|S|is|IS|es|ES|#|b)?([dD]ur|m|mi|min|moll|M|maj|aug|dim|\\+)?" // Base
				"([0-9/(#+\\-][0-9a-zA-Z/()#+\\-]*)?" // Extra
				"(?:/([a-hA-H])(s|S|is|IS|es|ES|#|b)?)?" // Inversions
				"$"
				);

	enum MatchPart {
		mpWhole,
		mpBaseNote, mpBaseNoteModifier,
		mpQuality, mpExtra,
		mpInversionNote, mpInversionNoteModifier
	};

	static const QHash<QChar, int> notePitches {
		{'c', 0}, {'d', 2}, {'e', 4}, {'f', 5}, {'g', 7}, {'a', 9}, {'b', 11}, {'h', 11}
	};

	static const QHash<QString, int> noteModifierEffects {
		{"s", -1}, {"S", -1}, {"es", -1}, {"ES", -1}, {"b", -1},
		{"is", 1}, {"IS", 1}, {"#", 1}
	};

	static const QHash<QString, Quality> qualities {
		{"dur", cvDur}, {"Dur", cvDur},
		{"m", cvMoll}, {"mi", cvMoll}, {"min", cvMoll},
		{"M", cvMaj}, {"maj", cvMaj},
		{"aug", cvAug},
		{"dim", cvDim}
	};

	QRegularExpressionMatch m = chordRegex.match(str);

	isValid_ = m.hasMatch();
	if(!isValid_)
		return;

	// Base note
	QChar baseNoteChar = m.captured(mpBaseNote)[0];

	if(baseNoteChar.isLower()) {
		quality_ = cvMoll;
		baseNote_ = notePitches[baseNoteChar];
	} else {
		quality_ = cvDur;
		baseNote_ = notePitches[baseNoteChar.toLower()];
	}

	// Base note modifier
	if(m.capturedLength(mpBaseNoteModifier))
		baseNote_ += noteModifierEffects[m.captured(mpBaseNoteModifier)];

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

QString Chord::toString() const
{
	static const QString variantStrings[_cvCount] = {"", "m", "maj", "aug", "dim"};
	static const QString noteNames[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "H"};

	if(!isValid_)
		return "###";

	QString result = noteNames[baseNote_] + variantStrings[quality_] + extra_;

	if(inversionNote_ != -1) {
		result += "/";
		result += noteNames[inversionNote_];
	}

	return result;
}

QVector<ChordInSong> songChords(const QString &song)
{
	QVector<ChordInSong> result;
	QRegularExpressionMatchIterator it = songChordRegex().globalMatch(song);

	while(it.hasNext()) {
		QRegularExpressionMatch m = it.next();
		const Chord ch(m.captured(2));

		if(!ch.isValid())
			continue;

		result.append(ChordInSong{ch, m.capturedStart(), m.capturedLength()});
	}

	return result;
}

void transposeSong(QString &song, int by)
{
	int posCorrection = 0;

	for(ChordInSong &chs : songChords(song)) {
		QString newChord = QString("[%1]").arg(chs.chord.transposed(by).toString());

		song.replace(chs.annotationPos + posCorrection, chs.annotationLength, newChord);
		posCorrection += newChord.length() - chs.annotationLength;
	}
}

const QRegularExpression &songChordRegex()
{
	static const QRegularExpression result("(\\[)([a-zA-Z0-9()\\-#]+)(\\])");
	return result;
}
