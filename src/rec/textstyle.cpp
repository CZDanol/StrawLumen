#include "textstyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>

void TextStyle::drawText(QPainter &p, const QRect &rect, const QString &str, const QTextOption &option, int flags) const
{
	p.save();
	p.translate(rect.topLeft());

	const QFontMetrics metrics(font);

	qreal hAlignConst;
	if(option.alignment() & Qt::AlignHCenter)
		hAlignConst = 0.5;
	else if(option.alignment() & Qt::AlignRight)
		hAlignConst = 1;
	else
		hAlignConst = 0;

	qreal vAlignConst;
	if(option.alignment() & Qt::AlignVCenter)
		vAlignConst = 0.5;
	else if(option.alignment() & Qt::AlignBottom)
		vAlignConst = 1;
	else
		vAlignConst = 0;

	QPainterPath path;
	QSize size;

	// Lay out lines
	for(const QString &line : str.split('\n')) {
		const int lineWidth = metrics.horizontalAdvance(line);
		if(lineWidth > size.width())
			size.setWidth(lineWidth);

		size.setHeight(size.height() + metrics.lineSpacing());
		path.addText(-lineWidth*hAlignConst, size.height(), font, line);
	}

	const QRectF pathBoundingRect = path.boundingRect();

	qreal scaleFactor = 1;
	if((flags & fScaleDownToFitRect) && (pathBoundingRect.width() > rect.width() || pathBoundingRect.height() > rect.height()))
		scaleFactor = qMin(rect.width()/pathBoundingRect.width(), rect.height()/pathBoundingRect.height());

	p.translate(rect.width()*hAlignConst, rect.height()*vAlignConst);
	p.scale(scaleFactor, scaleFactor);
	p.translate(-pathBoundingRect.left()-pathBoundingRect.width()*hAlignConst, -pathBoundingRect.top()-pathBoundingRect.height()*vAlignConst);

	if(backgroundEnabled) {
		const qreal m = backgroundPadding;
		p.fillRect(pathBoundingRect.marginsAdded(QMarginsF(m, m, m, m)), backgroundColor);
	}

	if(outlineEnabled)
		p.strokePath(path, QPen(outlineColor, outlineWidth/scaleFactor));

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
