#include "textstylepreviewwidget.h"

#include <QFontMetrics>
#include <QPainter>
#include <QTransform>

TextStylePreviewWidget::TextStylePreviewWidget(QWidget *parent) : QFrame(parent) {
	previewText_ = tr("Příliš žluťoučký kůň úpěl ďábelské ódy", "Preview sample text");
}

void TextStylePreviewWidget::setTextStyle(const TextStyle &style) {
	if(textStyle_ == style && !wasFirstUpdate_)
		return;

	forceSetTextStyle(style);
}

void TextStylePreviewWidget::forceSetTextStyle(const TextStyle &style) {
	textStyle_ = style;
	wasFirstUpdate_ = true;
	update();
}

void TextStylePreviewWidget::paintEvent(QPaintEvent *e) {
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);
	p.setPen(Qt::transparent);

	// Checkerboard pattern
	{
		static const QColor c1("#ddd"), c2("#aaa");

		p.fillRect(rect(), c1);

		QTransform m;
		m.scale(8, 8);

		p.save();
		p.setWorldTransform(m, true);
		p.setBrush(QBrush(c2, Qt::Dense4Pattern));
		p.drawRect(m.inverted().mapRect(rect()));
		p.restore();
	}

	textStyle_.drawText(p, rect(), previewText_, QTextOption(Qt::AlignCenter), 0);

	QFrame::paintEvent(e);
}
