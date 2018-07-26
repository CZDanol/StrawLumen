#ifndef WORDSPLIT_H
#define WORDSPLIT_H

#include <QString>

#include "rec/chord.h"

namespace WordSplit {

	enum Options : int {
		IncludeNewlines = 0b1
	};

	/// Returns positions of word divisions (indexes of first letters in the division), also returns chords in the string
	QVector<int> czech(QString str, ChordsInSong &chords, int options = 0);

}

#endif // WORDSPLIT_H
