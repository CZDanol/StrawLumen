#include "songcontentsyntaxhiglighter.h"

#include <QRegularExpressionMatchIterator>
#include <QTextDocument>

#include "util/chord.h"
#include "util/songsection.h"

SongContentSyntaxHiglighter::SongContentSyntaxHiglighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
	{
		chordFormat_.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
		chordFormat_.setForeground(Qt::blue);
		chordFormat_.setFontWeight(QFont::Bold);

		chordAnnotationSymbolFormat_.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
		//chordAnnotationSymbolFormat_.setForeground(Qt::gray);
	}

	{
		sectionFormat_.setFontWeight(QFont::Bold);
		sectionFormat_.setForeground(Qt::white);
		sectionFormat_.setBackground(Qt::black);

		sectionAnnotationSymbolFormat_.setForeground(Qt::transparent);
		sectionAnnotationSymbolFormat_.setBackground(Qt::black);
		sectionAnnotationSymbolFormat_.setFontLetterSpacing(400);
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
	static const QRegularExpression chordAnnotationRegex("(\\[)([^]]+)(\\])");
	it = chordAnnotationRegex.globalMatch(text);
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
	static const QRegularExpression sectionAnnotationRegex("(\\{)([^}]+)(\\})");
	it = sectionAnnotationRegex.globalMatch(text);
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
}
