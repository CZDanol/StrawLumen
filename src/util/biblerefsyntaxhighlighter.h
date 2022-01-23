#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class BibleRefSyntaxHighlighter : public QSyntaxHighlighter
{

public:
	explicit BibleRefSyntaxHighlighter(QTextDocument *document);

protected:
	void highlightBlock(const QString &text) override;

private:
	QTextCharFormat bookFormat_, verseFormat_, translationFormat_, errorFormat_;

};

