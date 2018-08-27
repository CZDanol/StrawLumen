#ifndef WORDCOMPLETINGLINEEDIT_H
#define WORDCOMPLETINGLINEEDIT_H

#include <QCompleter>
#include <QRegularExpression>

#include "wgt/extendedlineedit.h"

class WordCompletingLineEdit : public ExtendedLineEdit
{
	Q_OBJECT

public:
	explicit WordCompletingLineEdit(QWidget *parent = nullptr);

public:
	void setCompleter(QCompleter *c_);
	QCompleter *completer() const;

	void setCompleterSuffix(const QString &suffix);
	void setWordSeparator(const QRegularExpression &sep);

protected:
	void keyPressEvent(QKeyEvent *e);

private slots:
	void onCompleterActivated(const QString &replacement);
	void onCompleterHighlighted(const QString &replacement);

private:
	QString wordPrefixAtCursor() const;

private:
	QCompleter *c_ = nullptr;
	QRegularExpression wordSeparator_;
	QString completerSuffix_;

};

#endif // WORDCOMPLETINGLINEEDIT_H
