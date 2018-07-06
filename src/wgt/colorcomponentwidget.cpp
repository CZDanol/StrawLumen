#include "colorcomponentwidget.h"

#include <functional>

#include <QPainter>
#include <QMouseEvent>

#define BAR_PADDING 4

struct ComponentRecord {
	std::function<qreal(const QColor)> getterFunc;
	std::function<void(qreal, QColor&)> setterFunc;
};

static const ComponentRecord componentRecords[ColorComponentWidget::_ccCount] = {
	ComponentRecord{[](const QColor &c){return c.hueF();}, [](qreal val, QColor &c){c.setHsvF(val, c.saturationF(), c.valueF(), c.alphaF());}}, // H
	ComponentRecord{[](const QColor &c){return c.saturationF();}, [](qreal val, QColor &c){c.setHsvF(c.hueF(), val, c.valueF(), c.alphaF());}}, // S
	ComponentRecord{[](const QColor &c){return c.valueF();}, [](qreal val, QColor &c){c.setHsvF(c.hueF(), c.saturationF(), val, c.alphaF());}}, // V
	ComponentRecord{[](const QColor &c){return c.lightnessF();}, [](qreal val, QColor &c){c.setHslF(c.hueF(), c.saturationF(), val, c.alphaF());}}, // L

	ComponentRecord{[](const QColor &c){return c.redF();}, [](qreal val, QColor &c){c.setRedF(val);}}, // R
	ComponentRecord{[](const QColor &c){return c.greenF();}, [](qreal val, QColor &c){c.setGreenF(val);}}, // G
	ComponentRecord{[](const QColor &c){return c.blueF();}, [](qreal val, QColor &c){c.setBlueF(val);}}, // B

	ComponentRecord{[](const QColor &c){return c.alphaF();}, [](qreal val, QColor &c){c.setAlphaF(val);}} // A
};

ColorComponentWidget::ColorComponentWidget(QWidget *parent) : QWidget(parent)
{

}

void ColorComponentWidget::setComponent(ColorComponentWidget::ColorComponent component)
{
	if(component == component_)
		return;

	component_ = component;
	updateCache();
	update();
}

void ColorComponentWidget::setColor(const QColor &color)
{
	if(color == currentColor_)
		return;

	currentColor_ = color;
	updateCache();
	update();
}

void ColorComponentWidget::paintEvent(QPaintEvent *)
{
	const ComponentRecord &componentRecord = componentRecords[component_];

	if(backgroundCache_.size() != size())
		updateCache();

	QPainter p(this);
	p.drawPixmap(0, 0, backgroundCache_);
	p.setRenderHint(QPainter::Antialiasing);

	const qreal x = barRect_.left() + barRect_.width() * componentRecord.getterFunc(currentColor_);
	const qreal radius = (height()-2)/2;

	static const QPen pen(Qt::black);
	p.setPen(pen);
	p.setBrush(currentColor_);
	p.drawEllipse(QPointF(x, height()/2), radius, radius);
}

void ColorComponentWidget::mousePressEvent(QMouseEvent *e)
{
	QWidget::mousePressEvent(e);

	if(e->button() == Qt::LeftButton)
		pickColor(e->pos());
}

void ColorComponentWidget::mouseMoveEvent(QMouseEvent *e)
{
	QWidget::mouseMoveEvent(e);

	if(e->buttons() == Qt::LeftButton)
		pickColor(e->pos());
}

void ColorComponentWidget::updateCache()
{
	const ComponentRecord &componentRecord = componentRecords[component_];

	QImage backgroundCache(size(), QImage::Format_ARGB32);
	backgroundCache.fill(Qt::transparent);

	QPainter p(&backgroundCache);
	barRect_ = QRect(height()/2, BAR_PADDING, width()-height(), height() - BAR_PADDING*2);

	{
		const qreal progressInc = 1.0 / (qreal) barRect_.width();
		qreal progress = 0;
		QColor color = currentColor_;
		color.setAlphaF(1);
		QBrush brush = Qt::transparent;

		QRect currentRect(barRect_.left(), barRect_.top(), 1, barRect_.height());
		while(currentRect.left() <= barRect_.right()) {
			componentRecord.setterFunc(progress, color);
			brush.setColor(color);
			p.fillRect(currentRect, brush);

			currentRect.moveLeft(currentRect.left()+1);
			progress += progressInc;
		}
	}

	static const QPen pen("#555");
	p.setPen(pen);
	p.setBrush(Qt::transparent);
	p.drawRect(barRect_);

	p.end();

	backgroundCache_ = QPixmap::fromImage(backgroundCache);
}

void ColorComponentWidget::pickColor(QPoint &pos)
{
	const ComponentRecord &componentRecord = componentRecords[component_];

	componentRecord.setterFunc(
				qMax<qreal>(0,qMin<qreal>(1,(pos.x() - barRect_.left())/(qreal) barRect_.width())),
				currentColor_
				);
	update();
	emit sigColorChanged(currentColor_);
}
