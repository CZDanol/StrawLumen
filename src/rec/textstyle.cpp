#include "textstyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QJsonObject>

#include "job/jsonautomation.h"

TextStyle TextStyle::fromJSON(const QJsonValue &json)
{
	TextStyle result;
	result.loadFromJSON(json);
	return result;
}

void TextStyle::drawText(QPainter &p, const QRect &rect, const QString &str, const QTextOption &option, int flags) const
{
	if(str.isEmpty())
		return;

	const QFontMetrics metrics(font);

	qreal hAlignConst, vAlignConst;
	{
		if(option.alignment() & Qt::AlignHCenter)
			hAlignConst = 0.5;
		else if(option.alignment() & Qt::AlignRight)
			hAlignConst = 1;
		else
			hAlignConst = 0;

		if(option.alignment() & Qt::AlignVCenter)
			vAlignConst = 0.5;
		else if(option.alignment() & Qt::AlignBottom)
			vAlignConst = 1;
		else
			vAlignConst = 0;
	}

	QPainterPath path;
	QSize size;

	// Lay out lines
	for(const QString &line : str.split('\n')) {
		if(size.height())
			size.setHeight(size.height() + metrics.leading());

		const int lineWidth = metrics.horizontalAdvance(line);
		if(lineWidth > size.width())
			size.setWidth(lineWidth);

		size.setHeight(size.height() + metrics.ascent());
		path.addText(-lineWidth*hAlignConst, size.height(), font, line);
		size.setHeight(size.height() + metrics.descent());
	}
	QRectF pathBoundingRect = path.boundingRect();

	QSizeF availableSize = rect.size();
	if(outlineEnabled)
		availableSize -= QSizeF(outlineWidth*2,outlineWidth*2);

	qreal scaleFactor = 1;
	if((flags & fScaleDownToFitRect) && (pathBoundingRect.width() > availableSize.width() || pathBoundingRect.height() > availableSize.height())) {
		scaleFactor = qMin(availableSize.width()/pathBoundingRect.width(), availableSize.height()/pathBoundingRect.height());
	}

	p.save();
	p.translate(rect.left(), rect.top());
	p.translate(rect.width()*hAlignConst, rect.height()*vAlignConst);
	p.scale(scaleFactor, scaleFactor);
	p.translate(0, -size.height()*vAlignConst);

	if(backgroundEnabled) {
		p.fillRect(
					QRectF(QPointF(-size.width()*hAlignConst, 0), size)
					.marginsAdded(QMarginsF(backgroundPadding,backgroundPadding,backgroundPadding,backgroundPadding))
					,backgroundColor);
	}

	if(outlineEnabled) {
		p.setBrush(Qt::NoBrush);
		p.setPen(QPen(outlineColor, outlineWidth/scaleFactor, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		for(const auto &polygon : path.toSubpathPolygons())
			p.drawPolygon(polygon, Qt::WindingFill);
	}

	p.setBrush(color);
	p.setPen(Qt::NoPen);
	for(const auto &polygon : path.toFillPolygons())
		p.drawPolygon(polygon);

	p.restore();
}

void TextStyle::loadFromJSON(const QJsonValue &val)
{
	const QJsonObject json = val.toObject();

#define F(identifier, capitalizedIdentifier, Type, defaultValue)\
	identifier = defaultValue;\
	::loadFromJSON<Type>(identifier, json[#identifier]);

	TEXT_STYLE_FIELD_FACTORY(F)
#undef F

}

QJsonValue TextStyle::toJSON() const
{
	QJsonObject json;

#define F(identifier, capitalizedIdentifier, Type, defaultValue)\
	json[#identifier] = ::toJSON<Type>(identifier);

	TEXT_STYLE_FIELD_FACTORY(F)
#undef F

	return json;
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
