#include "presentationstylepreviewwidget.h"

#include <QPainter>
#include <QMatrix>

#include "job/settings.h"

PresentationStylePreviewWidget::PresentationStylePreviewWidget(QWidget *parent) : QWidget(parent)
{
	connect(&presentationStyle_, SIGNAL(sigNeedsRepaint()), this, SLOT(update()));
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

	QMatrix m;
	m.translate(previewRect.left(), previewRect.top());
	m.scale(scaleRatio, scaleRatio);

	p.save();
	p.setMatrix(m, true);

	presentationStyle_.drawSlide(p, QRect(QPoint(), screenRect.size()), tr("Příliš žluťoučký kůň\núpěl dábělské ódy.", "Slide preview main text"), tr("Název písně"));

	p.restore();

	// Show padding lines
	{
		p.setPen(Qt::white);
		p.setRenderHint(QPainter::Antialiasing, false);
		p.setCompositionMode(QPainter::CompositionMode_Exclusion);

		p.drawLine(m.map(QLine(presentationStyle_.leftPadding() * 0.01f * screenRect.width(), 0, presentationStyle_.leftPadding() * 0.01f * screenRect.width(), screenRect.height())));
		p.drawLine(m.map(QLine(screenRect.width() - presentationStyle_.rightPadding() * 0.01f * screenRect.width(), 0, screenRect.width() - presentationStyle_.rightPadding() * 0.01f * screenRect.width(), screenRect.height())));

		p.drawLine(m.map(QLine(0, presentationStyle_.topPadding() * 0.01f * screenRect.height(), screenRect.width(), presentationStyle_.topPadding() * 0.01f * screenRect.height())));
		p.drawLine(m.map(QLine(0, screenRect.height() - presentationStyle_.bottomPadding() * 0.01f * screenRect.height(), screenRect.width(), screenRect.height() - presentationStyle_.bottomPadding() * 0.01f * screenRect.height())));

		p.drawLine(m.map(QLine(0, screenRect.height() - (presentationStyle_.bottomPadding() + presentationStyle_.titleTextPadding()) * 0.01f * screenRect.height(), screenRect.width(), screenRect.height() - (presentationStyle_.bottomPadding() + presentationStyle_.titleTextPadding()) * 0.01f * screenRect.height())));
	}

}
