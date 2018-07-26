#ifndef WORDSPLIT_H
#define WORDSPLIT_H

#include <QString>

#include "rec/chord.h"

namespace WordSplit {

	/// Returns positions of word divisions (indexes of first letters in the division), also returns chords in the string
	QVector<int> czech(QString str, ChordsInSong &chords);

}

#endif // WORDSPLIT_H
