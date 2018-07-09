#include "songcontentsyntaxhiglighter.h"

#include <QRegularExpressionMatchIterator>
#include <QTextDocument>

#include "rec/chord.h"
#include "rec/songsection.h"

SongContentSyntaxHiglighter::SongContentSyntaxHiglighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
	{
		chordFormat_.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
		chordFormat_.setForeground(Qt::blue);
		chordFormat_.setFontWeight(QFont::Bold);

		chordAnnotationSymbolFormat_.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
		chordAnnotationSymbolFormat_.setForeground(Qt::gray);
	}

	{
		sectionFormat_.setFontWeight(QFont::Bold);
		sectionFormat_.setForeground(Qt::white);
		sectionFormat_.setBackground(Qt::black);

		sectionAnnotationSymbolFormat_.setForeground(Qt::transparent);
		sectionAnnotationSymbolFormat_.setBackground(Qt::black);
		//sectionAnnotationSymbolFormat_.setFontStretch(400);
	}

	{
		slideSeparatorFormat_.setFontWeight(QFont::Bold);
		slideSeparatorFormat_.setForeground(Qt::white);
		slideSeparatorFormat_.setBackground(QColor("#555"));
	}

	invalidAnnotationFormat_.setForeground(Qt::red);
}

void SongContentSyntaxHiglighter::highlightBlock(const QString &text)
{
	// Default font changes, we must update
	auto size = document()->defaultFont().pointSize() * 1.3;
	chordFormat_.setFontPointSize(size);
	chordAnnotationSymbolFormat_.setFontPointSize(size);

	QRegularExpressionMatchIterator it;

	// Match chords
	it = songChordRegex().globalMatch(text);
	while(it.hasNext()) {
		const QRegularExpressionMatch m = it.next();
		const Chord chord(m.captured(2));

		if(!chord.isValid()) {
			setFormat(m.capturedStart(), m.capturedLength(), invalidAnnotationFormat_);
			continue;
		}

		setFormat(m.capturedStart(1), m.capturedLength(1), chordAnnotationSymbolFormat_);
		setFormat(m.capturedStart(2), m.capturedLength(2), chordFormat_);
		setFormat(m.capturedStart(3), m.capturedLength(3), chordAnnotationSymbolFormat_);
	}

	// Match sections
	it = songSectionAnnotationRegex().globalMatch(text);
	while(it.hasNext()) {
		const QRegularExpressionMatch m = it.next();
		const SongSection songSection(m.captured(2));

		if(!songSection.isValid()) {
			setFormat(m.capturedStart(), m.capturedLength(), invalidAnnotationFormat_);
			continue;
		}

		setFormat(m.capturedStart(1), m.capturedLength(1), sectionAnnotationSymbolFormat_);
		setFormat(m.capturedStart(2), m.capturedLength(2), sectionFormat_);
		setFormat(m.capturedStart(3), m.capturedLength(3), sectionAnnotationSymbolFormat_);
	}

	// Match slide separators
	it = songSlideSeparatorRegex().globalMatch(text);
	while(it.hasNext()) {
		const QRegularExpressionMatch m = it.next();
		setFormat(m.capturedStart(), m.capturedLength(), slideSeparatorFormat_);
	}
}
