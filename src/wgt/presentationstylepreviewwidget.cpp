#include "presentationstylepreviewwidget.h"

#include <QPainter>

#include "gui/settingsdialog.h"
#include "job/backgroundmanager.h"

PresentationStylePreviewWidget::PresentationStylePreviewWidget(QWidget *parent) : QWidget(parent)
{
	connect(backgroundManager, &BackgroundManager::sigBackgroundLoaded, this, &PresentationStylePreviewWidget::onBackgroundManagerBackgroundLoaded);
}

void PresentationStylePreviewWidget::setPresentationStyle(const PresentationStyle &style)
{
	presentationStyle_ = style;
	update();
}

void PresentationStylePreviewWidget::paintEvent(QPaintEvent *)
{
	QRect screenRect = settingsDialog->projectionDisplayGeometry();
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

void PresentationStylePreviewWidget::onBackgroundManagerBackgroundLoaded(const QString &filename)
{
	if(presentationStyle_.usesBackground(filename))
		update();
}
