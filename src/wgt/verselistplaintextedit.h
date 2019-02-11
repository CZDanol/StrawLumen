#ifndef VERSELISTPLAINTEXTEDIT_H
#define VERSELISTPLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include <QCompleter>

class VerseListPlainTextEdit : public QPlainTextEdit
{
	Q_OBJECT

public:
	explicit VerseListPlainTextEdit(QWidget *parent = nullptr);

private:
	QCompleter *completer_;

};

#endif // VERSELISTPLAINTEXTEDIT_H
