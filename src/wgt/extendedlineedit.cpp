#include "extendedlineedit.h"

#include <QKeyEvent>

ExtendedLineEdit::ExtendedLineEdit(QWidget *parent) : QLineEdit(parent) {
}

void ExtendedLineEdit::focusInEvent(QFocusEvent *e) {
	QLineEdit::focusInEvent(e);
	emit sigFocused();
}

void ExtendedLineEdit::keyPressEvent(QKeyEvent *e) {
	if(e->key() == Qt::Key_Up)
		emit sigUpPressed();

	else if(e->key() == Qt::Key_Down)
		emit sigDownPressed();

	else if(!e->text().isEmpty())
		emit sigTextTyped(e->text());

	QLineEdit::keyPressEvent(e);
}
