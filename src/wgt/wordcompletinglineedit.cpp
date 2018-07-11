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

	QObject::connect(c_, SIGNAL(activated(QString)), this, SLOT(onCompleterActivated(QString)));
	QObject::connect(c_, SIGNAL(highlighted(QString)), this, SLOT(onCompleterHighlighted(QString)));
}

void WordCompletingLineEdit::setCompleterSuffix(const QString &suffix)
{
	completerSuffix_ = suffix;
}

void WordCompletingLineEdit::keyPressEvent(QKeyEvent *e)
{
	QLineEdit::keyPressEvent(e);

	if (!c_)
		return;

	//if (c_->completionPrefix().length() < 1)
	//	return c_->popup()->hide();

	c_->setCompletionPrefix(wordPrefixAtCursor());

	static const QRegularExpression wordRegexp("^\\w+$", QRegularExpression::UseUnicodePropertiesOption);

	if(wordRegexp.match(e->text()).hasMatch() || (e->key() == Qt::Key_Space && e->modifiers() & Qt::ControlModifier)) {
		QRect rect = cursorRect();
		rect.setWidth(c_->popup()->sizeHintForColumn(0) + c_->popup()->verticalScrollBar()->sizeHint().width());
		c_->complete(rect);
	}

	//if(c_->popup()->currentIndex().row() == -1)
	//	c_->popup()->setCurrentIndex(c_->completionModel()->index(0,0));
}

void WordCompletingLineEdit::onCompleterActivated(const QString &replacement)
{
	static const QRegularExpression nonwordRegexp("\\W", QRegularExpression::UseUnicodePropertiesOption);

	QString text_ = text();
	const int cursorPos = cursorPosition();
	const int start = text_.left(cursorPos).lastIndexOf(nonwordRegexp) + 1;

	QString realReplacement = replacement;
	int end = cursorPos + text_.mid(cursorPos).indexOf(nonwordRegexp);
	if(end < cursorPos) {
		end = text_.length();
		realReplacement += completerSuffix_;
	}

	text_.replace(start, end-start, realReplacement);

	setText(text_);
	setCursorPosition(start + realReplacement.length());
}

void WordCompletingLineEdit::onCompleterHighlighted(const QString &replacement)
{
	static const QRegularExpression nonwordRegexp("\\W", QRegularExpression::UseUnicodePropertiesOption);

	QString text_ = text();
	const int cursorPos = cursorPosition();
	const int start = text_.left(cursorPos).lastIndexOf(nonwordRegexp) + 1;

	int end = cursorPos + text_.mid(cursorPos).indexOf(nonwordRegexp);
	if(end < cursorPos)
		end = text_.length();

	text_.replace(start, end-start, replacement);

	setText(text_);
	setCursorPosition(start+replacement.size());
	cursorBackward(true, replacement.size() - (cursorPos-start));
}

QString WordCompletingLineEdit::wordPrefixAtCursor() const
{
	static const QRegularExpression nonwordRegexp("\\W", QRegularExpression::UseUnicodePropertiesOption);

	const QString text_ = text();
	const int cursorPos = cursorPosition();
	const int start = text_.left(cursorPos).lastIndexOf(nonwordRegexp) + 1;

	return text_.mid(start, cursorPos - start);
}
