#include "presentationbackgroundpreviewwidget.h"

#include <QPainter>

#include "job/settings.h"

PresentationBackgroundPreviewWidget::PresentationBackgroundPreviewWidget(QWidget *parent) : QWidget(parent)
{
	connect(&presentationBackground_, SIGNAL(sigChanged()), this, SLOT(update()));
}

void PresentationBackgroundPreviewWidget::setPresentationBackground(const PresentationBackground &background)
{
	presentationBackground_ = background;
}

void PresentationBackgroundPreviewWidget::paintEvent(QPaintEvent *)
{
	QRect previewRect;
	previewRect.setSize(settings->projectionDisplayGeometry().size().scaled(size(), Qt::KeepAspectRatio));
	previewRect.moveCenter(rect().center());

	QPainter p(this);
	p.setRenderHint(QPainter::SmoothPixmapTransform);
	presentationBackground_.draw(p, previewRect);
}
