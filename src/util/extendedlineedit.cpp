#include "extendedlineedit.h"

ExtendedLineEdit::ExtendedLineEdit(QWidget *parent) : QLineEdit(parent)
{

}

void ExtendedLineEdit::focusInEvent(QFocusEvent *e)
{
	QLineEdit::focusInEvent(e);
	emit sigFocused();
}
