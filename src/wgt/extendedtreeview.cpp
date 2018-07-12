#include "extendedtreeview.h"

#include <QKeyEvent>

ExtendedTreeView::ExtendedTreeView(QWidget *parent) : QTreeView(parent)
{

}

void ExtendedTreeView::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Up)
		emit sigUpPressed();

	else if(e->key() == Qt::Key_Down)
		emit sigDownPressed();

	QTreeView::keyPressEvent(e);
}
