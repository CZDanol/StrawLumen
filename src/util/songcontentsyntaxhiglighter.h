#ifndef SONGCONTENTSYNTAXHIGLIGHTER_H
#define SONGCONTENTSYNTAXHIGLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class SongContentSyntaxHiglighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	SongContentSyntaxHiglighter(QTextDocument *parent = nullptr);

public:
	void setSepSyllables(bool set);
	bool sepSyllables() const;

protected:
	void highlightBlock(const QString &text) override;

private:
	QTextCharFormat chordFormat_, chordAnnotationSymbolFormat_;
	QTextCharFormat sectionFormat_, sectionAnnotationSymbolFormat_;
	QTextCharFormat slideSeparatorFormat_;

private:
	QTextCharFormat syllableFormat_, syllableAltFormat_;

private:
	QTextCharFormat invalidAnnotationFormat_;
	QTextCharFormat invalidWhitespaceFormat_;

private:
	bool sepSyllables_ = false;

};

#endif // SONGCONTENTSYNTAXHIGLIGHTER_H
