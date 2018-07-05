#include "chord.h"

#include <QRegularExpression>

Chord::Chord(const QString &str)
{
	static const QRegularExpression chordRegex(
				"^"
				"(([a-hA-H])(s|is|es|#|b)?(m|mi|min|M|maj|aug|dim|\\+)?(6|7|/maj7|\\(maj7\\))?)" // Base
				"(?:\\s*/\\s*([a-hA-H])(s|is|es|#|b)?)?" // Inversions
				"$"
				);

	enum MatchPart {
		mpWhole,
		mpWholeBase,
		mpBaseNote, mpBaseNoteHalfStep, mpBaseComponent, mpBaseAdded,
		mpInversionNote, mpInversionNoteHalfStep
	};

	QRegularExpressionMatch ma = chordRegex.match(str);

	isValid = ma.hasMatch();
	if(!isValid)
		return;
}
