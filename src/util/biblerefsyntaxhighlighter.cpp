#include "biblerefsyntaxhighlighter.h"

#include "rec/bibleref.h"

BibleRefSyntaxHighlighter::BibleRefSyntaxHighlighter(QTextDocument *document) : QSyntaxHighlighter(document)
{
	{
		bookFormat_.setFontWeight(QFont::Bold);
	}

	{

	}

	{
		translationFormat_.setForeground(Qt::white);
		translationFormat_.setBackground(QColor("#333"));
	}

	{
		errorFormat_.setForeground(Qt::red);
	}
}

void BibleRefSyntaxHighlighter::highlightBlock(const QString &text)
{
	int pos = 0;
	for(const QString &line : text.split('\n')) {
		const QRegularExpressionMatch m = BibleRef::regex().match(line);
		if(m.hasMatch() && BibleRef(line).isValid()) {
			setFormat(pos + m.capturedStart(1), m.capturedLength(1), bookFormat_);
			setFormat(pos + m.capturedStart(2), m.capturedEnd(3) - m.capturedStart(2), verseFormat_);
			setFormat(pos + m.capturedStart(4), m.capturedLength(4), translationFormat_);
		}
		else
			setFormat(pos, line.length(), errorFormat_);

		pos += line.length();
	}
}
