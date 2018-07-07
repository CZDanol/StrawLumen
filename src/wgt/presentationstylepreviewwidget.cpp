#include "presentationstylepreviewwidget.h"

PresentationStylePreviewWidget::PresentationStylePreviewWidget(QWidget *parent) : QWidget(parent)
{

}

void PresentationStylePreviewWidget::setPresentationStyle(const PresentationStyle &style)
{
	style_ = style;
	update();
}
