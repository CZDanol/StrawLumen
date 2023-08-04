#ifndef CHORD_H
#define CHORD_H

#include <QString>
#include <QVector>
#include <QRegularExpression>
#include <QTextCursor>

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
    static const QRegularExpression &chordRegex();

    Chord();
    explicit Chord(const QString &str);

public:
    bool isValid() const;
    bool isFlat() const;

    Chord transposed(int by) const;

    QString toString(bool flatVariant = false) const;

private:
    bool isValid_, isFlat_;
    int baseNote_;
    Quality quality_;
    QString extra_;
    int inversionNote_; ///< -1 if no inversion

};

struct ChordInSong {
    Chord chord;
    qsizetype annotationPos, annotationLength;
};
using ChordsInSong = QVector<ChordInSong>;

/// Captures: 0 - whole, 1 - annotation prefix, 2 - chord, 3 - annotation suffix
const QRegularExpression &songChordAnnotationRegex();

/// Returns all valid chords in the song
ChordsInSong songChords(const QString &song);

/// Returns chord that is around (its annotation includes pos) the specified position or invalid chord
ChordInSong chordAroundPos(const QString &song, int pos);

/// Returns all chords that are inside the selection (at least partially)
ChordsInSong chordsInsideSelection(const QString &song, int selectionStart, int selectionEnd);
ChordsInSong chordsInsideSelection(const QTextCursor &textCursor);

/// Removes all chord annotations from the song
QString removeSongChords(const QString &song);

/// Removes all chord annotations from the song and stores them in the chords variable
QString removeSongChords(const QString &song, QVector<ChordInSong> &chords);

/// Transposes all valid chords in the song. All chords are changed to standard format (even when by=0)
void transposeSong(QString &song, int by, bool flatChords = false);

/// Copies chords from source and inserts them into target (based on syllable count)
QString copySongChords(const QString &source, const QString &target);

#endif // CHORD_H
