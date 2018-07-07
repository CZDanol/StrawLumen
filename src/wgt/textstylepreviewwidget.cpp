#include "textstylepreviewwidget.h"

#include <QPainter>
#include <QFontMetrics>

TextStylePreviewWidget::TextStylePreviewWidget(QWidget *parent) : QFrame(parent)
{
	previewText_ = tr("Droboučký kůň...");
}

void TextStylePreviewWidget::setTextStyle(const TextStyle &style)
{
	if(textStyle_ == style && !wasFirstUpdate_)
		return;

	forceSetTextStyle(style);
}

void TextStylePreviewWidget::forceSetTextStyle(const TextStyle &style)
{
	textStyle_ = style;
	previewTextRect_ = QFontMetrics(textStyle_.font).boundingRect(previewText_);
	wasFirstUpdate_ = true;

	textPath_ = QPainterPath();
	textPath_.addText(-previewTextRect_.topLeft(), textStyle_.font, previewText_);

	update();
}

void TextStylePreviewWidget::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);
	p.setPen(Qt::transparent);

	// Checkerboard pattern
	{
		static const QColor c1("#ddd"), c2("#aaa");

		p.fillRect(rect(), c1);

		QMatrix m;
		m.scale(8,8);

		p.save();
		p.setWorldMatrix(m, true);
		p.setBrush(QBrush(c2, Qt::Dense4Pattern));
		p.drawRect(m.inverted().mapRect(rect()));
		p.restore();
	}

	QRect textRect;
	textRect.setSize(previewTextRect_.size());
	textRect.moveCenter(rect().center());

	if(textStyle_.backgroundEnabled)
		p.fillRect(textRect.marginsAdded(QMargins(textStyle_.backgroundPadding, textStyle_.backgroundPadding, textStyle_.backgroundPadding, textStyle_.backgroundPadding)), textStyle_.backgroundColor);

	p.translate(textRect.topLeft());

	if(textStyle_.outlineEnabled)
		p.strokePath(textPath_, QPen(textStyle_.outlineColor, textStyle_.outlineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	p.fillPath(textPath_, textStyle_.color);

	QFrame::paintEvent(e);
}
