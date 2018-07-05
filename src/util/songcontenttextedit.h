#ifndef SONGCONTENTTEXTEDIT_H
#define SONGCONTENTTEXTEDIT_H

#include <QTextEdit>
#include <QKeyEvent>

class SongContentSyntaxHiglighter;

class SongContentTextEdit : public QTextEdit
{
	Q_OBJECT

public:
	explicit SongContentTextEdit(QWidget *parent = nullptr);

protected:
	void keyPressEvent(QKeyEvent *e) override;

private:
	SongContentSyntaxHiglighter *contentSyntaxHiglighter_;

};

#endif // SONGCONTENTTEXTEDIT_H
