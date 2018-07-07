#include "textstyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>

QPoint rectPointByAlign(const QRect &rect, int align) {
	QPoint result = rect.topLeft();

	if(align & Qt::AlignHCenter)
		result.setX(rect.center().x());
	else if(align & Qt::AlignRight)
		result.setX(rect.right());

	if(align & Qt::AlignVCenter)
		result.setY(rect.center().y());
	else if(align & Qt::AlignBottom)
		result.setY(rect.bottom());

	return result;
}

void TextStyle::drawText(QPainter &p, const QRect &rect, const QString &str, int align, int flags)
{
	QFontMetrics fontMetrics(font);

	QPainterPath path;
	qreal x, height = 0, width = 0;

	for(const QString &line : str.split('\n')) {
		height += fontMetrics.lineSpacing();

		const QRect lineRect = fontMetrics.boundingRect(line);
		const qreal lineWidth = lineRect.width();

		if(align & Qt::AlignHCenter)
			x = -lineRect.right()/2;
		else if(align & Qt::AlignRight)
			x = -lineRect.right();
		else
			x = 0;

		if(lineWidth > width)
			width = lineWidth;

		path.addText(x, height, font, line);
	}

	qreal y;
	if(align & Qt::AlignVCenter)
		y = -height / 2;
	else if(align & Qt::AlignBottom)
		y = -height;
	else
		y = 0;


	p.save();
	p.translate(rectPointByAlign(rect, align));

	if(flags & fScaleDownToFitRect && (rect.width() < width || rect.height() < height)) {
		const qreal scaleFactor = qMin(rect.width()/width, rect.height()/height);
		p.scale(scaleFactor, scaleFactor);
	}

	p.translate(0, y);

	if(backgroundEnabled)
		p.fillRect(x, y, width, height, backgroundColor);

	if(outlineEnabled)
		p.strokePath(path, QPen(outlineColor, outlineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	p.fillPath(path, color);

	p.restore();
}

bool TextStyle::operator==(const TextStyle &other) const
{
	return
			font == other.font &&
			color == other.color &&

			outlineEnabled == other.outlineEnabled &&
			outlineColor == other.outlineColor &&
			outlineWidth == other.outlineWidth &&

			backgroundEnabled == other.backgroundEnabled &&
			backgroundColor == other.backgroundColor &&
			backgroundPadding == other.backgroundPadding
			;
}
