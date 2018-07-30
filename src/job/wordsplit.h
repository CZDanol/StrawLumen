#ifndef WORDSPLIT_H
#define WORDSPLIT_H

#include <QString>

#include "rec/chord.h"

namespace WordSplit {

	enum Options : int {
		IncludeNewlines = 0b1,
		IncludeChords = 0b10
	};

	/// Returns positions of word divisions (indexes of first letters in the division), also returns chords in the string
	QVector<int> czech(const QString &src, ChordsInSong &chords, int options = 0);

	QString splitVisualization(const QString &str, const QVector<int> &splits);

}

#endif // WORDSPLIT_H
