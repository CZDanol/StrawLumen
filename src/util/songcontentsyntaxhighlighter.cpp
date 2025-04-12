#include "songcontentsyntaxhighlighter.h"

#include <QPalette>
#include <QRegularExpressionMatchIterator>
#include <QTextDocument>

#include "job/wordsplit.h"
#include "main.h"
#include "rec/chord.h"
#include "rec/songsection.h"

SongContentSyntaxHighlighter::SongContentSyntaxHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
	{
		chordFormat_.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
		// chordFormat_.setForeground(Qt::blue);
		chordFormat_.setFontWeight(QFont::Bold);

		chordAnnotationSymbolFormat_.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
		chordAnnotationSymbolFormat_.setForeground(Qt::gray);
	}

	{
		sectionFormat_.setFontWeight(QFont::Bold);
		sectionFormat_.setForeground(Qt::white);

		sectionAnnotationSymbolFormat_.setForeground(Qt::transparent);
		//sectionAnnotationSymbolFormat_.setFontStretch(400);
	}

	{
		slideSeparatorFormat_.setFontWeight(QFont::Bold);
		slideSeparatorFormat_.setForeground(Qt::white);
		slideSeparatorFormat_.setBackground(QColor("#555"));
	}

	{
		syllableFormat_.setBackground(QColor(0, 255, 0, 64));
		syllableAltFormat_.setBackground(QColor(0, 0, 255, 64));
	}
}

void SongContentSyntaxHighlighter::setSepSyllables(bool set) {
	if(sepSyllables_ == set)
		return;

	sepSyllables_ = set;
	rehighlight();
}

bool SongContentSyntaxHighlighter::sepSyllables() const {
	return sepSyllables_;
}

void SongContentSyntaxHighlighter::highlightBlock(const QString &text) {
	// Update palette based on dark/light mode
	{
		QPalette p;
		chordFormat_.setForeground(p.link());

		const QColor bgColor = isDarkMode ? QColor("#333") : Qt::black;
		sectionFormat_.setBackground(bgColor);
		sectionAnnotationSymbolFormat_.setBackground(bgColor);

		const QColor errColor = isDarkMode ? QColor("#f55") : Qt::red;
		invalidWhitespaceFormat_.setBackground(errColor);
		invalidAnnotationFormat_.setForeground(errColor);

		anyWhitespaceFormat_.setBackground(isDarkMode ? QColor("#22ffffff") : QColor("#22000000"));

		// Default font changes, we must update
		auto size = document()->defaultFont().pointSize() * 1.3;
		chordFormat_.setFontPointSize(size);
		chordAnnotationSymbolFormat_.setFontPointSize(size);
	}

	// Sep syllables
	if(sepSyllables_) {
		bool alt = false;
		int prev = 0;
		ChordsInSong chords;
		QVector<int> splits = WordSplit::czech(text, chords, WordSplit::IncludeNewlines | WordSplit::IncludeChords);

		for(int split: splits) {
			setFormat(prev, split, alt ? syllableAltFormat_ : syllableFormat_);
			alt = !alt;
			prev = split;
		}

		setFormat(prev, text.length(), alt ? syllableAltFormat_ : syllableFormat_);
	}

	QRegularExpressionMatchIterator it;

	// Match chords
	it = songChordAnnotationRegex().globalMatch(text);
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

	// Visualise all spaces
	static const QRegularExpression spacesRegex("\\s+");
	it = spacesRegex.globalMatch(text);
	while(it.hasNext()) {
		const QRegularExpressionMatch m = it.next();
		setFormat(m.capturedStart(), m.capturedLength(), anyWhitespaceFormat_);
	}

	// Warn about double spaces
	static const QRegularExpression doubleSpaceRegex("\t+|  +|^\\s+|\\s+$");
	it = doubleSpaceRegex.globalMatch(text);
	while(it.hasNext()) {
		const QRegularExpressionMatch m = it.next();
		setFormat(m.capturedStart(), m.capturedLength(), invalidWhitespaceFormat_);
	}
}
