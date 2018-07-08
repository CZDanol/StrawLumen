#include "presentationbackgroundpreviewwidget.h"

#include <QPainter>

#include "gui/settingsdialog.h"
#include "job/backgroundmanager.h"

PresentationBackgroundPreviewWidget::PresentationBackgroundPreviewWidget(QWidget *parent) : QWidget(parent)
{
	connect(backgroundManager, SIGNAL(sigBackgroundLoaded(QString)), this, SLOT(onBackgroundMangerBackgroundLoaded(QString)));
}

void PresentationBackgroundPreviewWidget::setPresentationBackground(const PresentationBackground &background)
{
	/*if(presentationBackground_ == background)
		return;*/

	presentationBackground_ = background;
	update();
}

void PresentationBackgroundPreviewWidget::paintEvent(QPaintEvent *)
{
	QRect previewRect;
	previewRect.setSize(settingsDialog->projectionDisplayGeometry().size().scaled(size(), Qt::KeepAspectRatio));
	previewRect.moveCenter(rect().center());

	QPainter p(this);
	p.setRenderHint(QPainter::SmoothPixmapTransform);
	presentationBackground_.draw(p, previewRect);
}

void PresentationBackgroundPreviewWidget::onBackgroundMangerBackgroundLoaded(const QString &filename)
{
	// We might have been waiting for the image to load
	if(filename == presentationBackground_.filename)
		update();
}
