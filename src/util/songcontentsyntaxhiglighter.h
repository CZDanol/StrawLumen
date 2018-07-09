#ifndef SONGCONTENTSYNTAXHIGLIGHTER_H
#define SONGCONTENTSYNTAXHIGLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class SongContentSyntaxHiglighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	SongContentSyntaxHiglighter(QTextDocument *parent = nullptr);

protected:
	void highlightBlock(const QString &text) override;

private:
	QTextCharFormat chordFormat_, chordAnnotationSymbolFormat_;
	QTextCharFormat sectionFormat_, sectionAnnotationSymbolFormat_;
	QTextCharFormat slideSeparatorFormat_;

private:
	QTextCharFormat invalidAnnotationFormat_;

};

#endif // SONGCONTENTSYNTAXHIGLIGHTER_H
