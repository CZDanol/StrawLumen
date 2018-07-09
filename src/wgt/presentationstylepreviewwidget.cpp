#include "presentationstylepreviewwidget.h"

#include <QPainter>

#include "job/settings.h"

PresentationStylePreviewWidget::PresentationStylePreviewWidget(QWidget *parent) : QWidget(parent)
{
	connect(&presentationStyle_, SIGNAL(sigChanged()), this, SLOT(update()));
}

void PresentationStylePreviewWidget::setPresentationStyle(const PresentationStyle &style)
{
	// sigChanged is emitted, no need to call update
	presentationStyle_ = style;
}

void PresentationStylePreviewWidget::paintEvent(QPaintEvent *)
{
	QRect screenRect = settings->projectionDisplayGeometry();
	QRect previewRect;
	previewRect.setSize(screenRect.size().scaled(size(), Qt::KeepAspectRatio));
	previewRect.moveCenter(rect().center());

	qreal scaleRatio = previewRect.width() / (qreal) screenRect.width();

	QPainter p(this);
	p.setRenderHint(QPainter::SmoothPixmapTransform);
	p.setRenderHint(QPainter::Antialiasing);

	p.translate(previewRect.topLeft());
	p.scale(scaleRatio, scaleRatio);

	presentationStyle_.drawSlide(p, QRect(QPoint(), screenRect.size()), tr("Příliš žluťoučký kůň\núpěl dábělské ódy.", "Slide preview main text"), tr("Název písně"));
}
