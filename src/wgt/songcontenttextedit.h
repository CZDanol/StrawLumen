#ifndef SONGCONTENTTEXTEDIT_H
#define SONGCONTENTTEXTEDIT_H

#include <QKeyEvent>
#include <QTextEdit>

class SongContentSyntaxHighlighter;

class SongContentTextEdit : public QTextEdit {
	Q_OBJECT

public:
	explicit SongContentTextEdit(QWidget *parent = nullptr);

public:
	SongContentSyntaxHighlighter *syntaxHiglighter();

signals:
	void sigAltlLeftPressed();
	void sigAltRightPressed();

protected:
	void keyPressEvent(QKeyEvent *e) override;
	bool eventFilter(QObject *obj, QEvent *event) override;

private:
	SongContentSyntaxHighlighter *contentSyntaxHiglighter_;
};

#endif// SONGCONTENTTEXTEDIT_H
