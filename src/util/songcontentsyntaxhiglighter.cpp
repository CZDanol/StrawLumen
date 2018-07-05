#include "songcontentsyntaxhiglighter.h"

#include <QRegularExpressionMatchIterator>

#include "util/chord.h"
#include "util/songsection.h"

SongContentSyntaxHiglighter::SongContentSyntaxHiglighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
	chordFormat_.setForeground(Qt::blue);

	sectionFormat_.setFontWeight(QFont::Bold);

	annotationSymbolsFormat_.setForeground(Qt::gray);

	invalidAnnotationFormat_.setForeground(Qt::red);
}

void SongContentSyntaxHiglighter::highlightBlock(const QString &text)
{
	QRegularExpressionMatchIterator it;

	// Match chords
	static const QRegularExpression chordAnnotationRegex("(\\[)([^]]*)(\\])");
	it = chordAnnotationRegex.globalMatch(text);
	while(it.hasNext()) {
		const QRegularExpressionMatch m = it.next();
		const Chord chord(m.captured(2));

		setFormat(m.capturedStart(1), m.capturedLength(1), annotationSymbolsFormat_);
		setFormat(m.capturedStart(2), m.capturedLength(2), chord.isValid ? chordFormat_ : invalidAnnotationFormat_);
		setFormat(m.capturedStart(3), m.capturedLength(3), annotationSymbolsFormat_);
	}

	// Match sections
	static const QRegularExpression sectionAnnotationRegex("(\\{)([^}]*)(\\})");
	it = sectionAnnotationRegex.globalMatch(text);
	while(it.hasNext()) {
		const QRegularExpressionMatch m = it.next();
		const SongSection songSection(m.captured(2));

		setFormat(m.capturedStart(1), m.capturedLength(1), annotationSymbolsFormat_);
		setFormat(m.capturedStart(2), m.capturedLength(2), songSection.isValid() ? sectionFormat_ : invalidAnnotationFormat_);
		setFormat(m.capturedStart(3), m.capturedLength(3), annotationSymbolsFormat_);
	}
}
