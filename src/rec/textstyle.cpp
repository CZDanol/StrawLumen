#include "textstyle.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QJsonObject>
#include <QRegularExpression>
#include <QStringRef>
#include <QDebug>

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

    QSizeF availableSize = rect.size();
    if(outlineEnabled)
        availableSize -= QSizeF(outlineWidth*2,outlineWidth*2);

    QPainterPath path;
    QSize size;

    QStringRef remainingText = QStringRef(&str).trimmed();

    // Initial scale factor estimation
    //qreal scaleFactor = qMin(1.0, static_cast<qreal>(availableSize.width()) / (metrics.height() * approxLineCount + metrics.leading() * (approxLineCount-1)));
    //qreal scaleFactor = qMin(1.0, sqrt(qreal(metrics.height()) * qreal(metrics.horizontalAdvance(str)) / (availableSize.width() * availableSize.height())));
    qreal scaleFactor = qMin(1.0, sqrt(
                    (availableSize.width() * availableSize.height())
                    / (qreal(metrics.height()) * qreal(metrics.horizontalAdvance(str)))
                    ));

    const int approxAvailableWidth = static_cast<int>(availableSize.width() / scaleFactor);

    // Lay out lines
    while(!remainingText.isEmpty()) {
        int ix = remainingText.indexOf('\n');

        QStringRef lineRef = remainingText.left(ix).trimmed();
        QString line = lineRef.toString();

        if(size.height())
            size.setHeight(size.height() + metrics.leading());

        int lineWidth = metrics.horizontalAdvance(line);
        if(lineWidth > approxAvailableWidth && (flags & fWordWrap)) {
            // Calculate wrap points
            QVector<int> wrapPoints;
            static const QRegularExpression wrapPointsRegex(R"(\b(\p{L} )?(\p{L}|\p{M}|\p{P})+( …\p{P}*)?)", QRegularExpression::UseUnicodePropertiesOption);
            QRegularExpressionMatchIterator it = wrapPointsRegex.globalMatch(line);
            while(it.hasNext())
                wrapPoints += it.next().capturedEnd();

            // Binary search in the wrap points
            int start = 0, end = wrapPoints.size();
            while(start + 1 < end) {
                const int mid = (start + end) / 2;
                lineWidth = metrics.horizontalAdvance(line.left(wrapPoints[mid]));

                if(lineWidth > approxAvailableWidth)
                    end = mid;
                else
                    start = mid;
            }

            const int len = wrapPoints[start];
            line = line.left(len);
            lineWidth = metrics.horizontalAdvance(line);
            ix = lineRef.position() - remainingText.position() + len;
        }

        if(lineWidth > size.width())
            size.setWidth(lineWidth);

        size.setHeight(size.height() + metrics.ascent());
        path.addText(-lineWidth*hAlignConst, size.height(), font, line);
        size.setHeight(size.height() + metrics.descent());

        remainingText = ix == -1 ? nullptr : QStringRef(&str).mid(remainingText.position() + ix+1);
    }
    QRectF pathBoundingRect = path.boundingRect();

    if((flags & fScaleDownToFitRect) && (pathBoundingRect.width() > availableSize.width() || pathBoundingRect.height() > availableSize.height())) {
        scaleFactor = qMin(availableSize.width()/pathBoundingRect.width(), availableSize.height()/pathBoundingRect.height());
    }
    else
        scaleFactor = 1;

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
