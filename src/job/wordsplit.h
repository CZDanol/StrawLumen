#ifndef WORDSPLIT_H
#define WORDSPLIT_H

#include <QString>

namespace WordSplit {

	/// Returns positions of word divisions (indexes of first letters in the division)
	QVector<int> czech(QString str);

}

#endif // WORDSPLIT_H
