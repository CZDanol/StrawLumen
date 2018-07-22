#include "extendedtreeview.h"

#include <QKeyEvent>

ExtendedTreeView::ExtendedTreeView(QWidget *parent) : QTreeView(parent)
{
	startDragFunction_ = [](Qt::DropActions){ return false; };
}

void ExtendedTreeView::setStartDragFunction(const ExtendedTreeView::StartDragFunction &f)
{
	startDragFunction_ = f;
}

void ExtendedTreeView::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Up)
		emit sigUpPressed();

	else if(e->key() == Qt::Key_Down)
		emit sigDownPressed();

	else if(!e->text().isEmpty())
		emit sigTextTyped(e->text());

	QTreeView::keyPressEvent(e);
}

void ExtendedTreeView::startDrag(Qt::DropActions supportedActions)
{
	if(!startDragFunction_(supportedActions))
		QTreeView::startDrag(supportedActions);
}
