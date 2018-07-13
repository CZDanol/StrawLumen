#ifndef CHORD_H
#define CHORD_H

#include <QString>
#include <QVector>
#include <QRegularExpression>

class Chord
{

public:
	enum Quality {
		cvDur,
		cvMoll,
		cvMaj,
		cvAug,
		cvDim,
		_cvCount
	};

public:
	Chord();
	explicit Chord(const QString &str);

public:
	bool isValid() const;

	Chord transposed(int by) const;

	QString toString(bool flatVariant = false) const;

private:
	bool isValid_;
	int baseNote_;
	Quality quality_;
	QString extra_;
	int inversionNote_; ///< -1 if no inversion

};

struct ChordInSong {
	Chord chord;
	int annotationPos, annotationLength;
};
using ChordsInSong = QVector<ChordInSong>;

/// Captures: 0 - whole, 1 - annotation prefix, 2 - chord, 3 - annotation suffix
const QRegularExpression &songChordAnnotationRegex();

/// Returns all valid chords in the song
ChordsInSong songChords(const QString &song);

/// Transposes all valid chords in the song. All chords are changed to standard format (even when by=0)
void transposeSong(QString &song, int by, bool flatChords = false);

#endif // CHORD_H
