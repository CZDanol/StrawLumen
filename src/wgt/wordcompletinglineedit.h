#ifndef WORDCOMPLETINGLINEEDIT_H
#define WORDCOMPLETINGLINEEDIT_H

#include <QCompleter>

#include "wgt/extendedlineedit.h"

class WordCompletingLineEdit : public ExtendedLineEdit
{
	Q_OBJECT

public:
	explicit WordCompletingLineEdit(QWidget *parent = nullptr);

public:
	void setCompleter(QCompleter *c_);
	QCompleter *completer() const;

protected:
	void keyPressEvent(QKeyEvent *e);

private slots:
	void insertCompletion(const QString &replacement);

private:
	QString wordPrefixAtCursor() const;

private:
	QCompleter *c_ = nullptr;

};

#endif // WORDCOMPLETINGLINEEDIT_H
