#ifndef SONGCONTENTTEXTEDIT_H
#define SONGCONTENTTEXTEDIT_H

#include <QKeyEvent>
#include <QTextEdit>

class SongContentSyntaxHiglighter;

class SongContentTextEdit : public QTextEdit {
	Q_OBJECT

public:
	explicit SongContentTextEdit(QWidget *parent = nullptr);

public:
	SongContentSyntaxHiglighter *syntaxHiglighter();

signals:
	void sigAltlLeftPressed();
	void sigAltRightPressed();

protected:
	void keyPressEvent(QKeyEvent *e) override;
	bool eventFilter(QObject *obj, QEvent *event) override;

private:
	SongContentSyntaxHiglighter *contentSyntaxHiglighter_;
};

#endif// SONGCONTENTTEXTEDIT_H
