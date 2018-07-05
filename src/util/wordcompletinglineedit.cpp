#include "wordcompletinglineedit.h"

#include <QKeyEvent>
#include <QAbstractItemView>
#include <QScrollBar>

WordCompletingLineEdit::WordCompletingLineEdit(QWidget *parent) : ExtendedLineEdit(parent)
{

}

void WordCompletingLineEdit::setCompleter(QCompleter *completer)
{
	if (c_)
		QObject::disconnect(c_, 0, this, 0);

	c_ = completer;
	if (!c_)
		return;

	c_->setWidget(this);
	c_->setCompletionMode(QCompleter::PopupCompletion);
	c_->setCaseSensitivity(Qt::CaseInsensitive);

	QObject::connect(c_, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

void WordCompletingLineEdit::keyPressEvent(QKeyEvent *e)
{
	QLineEdit::keyPressEvent(e);

	if (!c_)
		return;

	c_->setCompletionPrefix(wordPrefixAtCursor());
	if (c_->completionPrefix().length() < 1)
		return c_->popup()->hide();

	QRect rect = cursorRect();
	rect.setWidth(c_->popup()->sizeHintForColumn(0) + c_->popup()->verticalScrollBar()->sizeHint().width());
	c_->complete(rect);

	if(c_->popup()->currentIndex().row() == -1)
		c_->popup()->setCurrentIndex(c_->completionModel()->index(0,0));
}

void WordCompletingLineEdit::insertCompletion(const QString &replacement)
{
	QString text_ = text();
	const int cursorPos = cursorPosition();
	const int start = text_.left(cursorPos).lastIndexOf(' ') + 1;

	text_.replace(start, cursorPos-start, replacement);

	setText(text_);
}

QString WordCompletingLineEdit::wordPrefixAtCursor() const
{
	const QString text_ = text();
	const int cursorPos = cursorPosition();
	const int start = text_.left(cursorPos).lastIndexOf(' ') + 1;

	return text_.mid(start, cursorPos - start);
}
