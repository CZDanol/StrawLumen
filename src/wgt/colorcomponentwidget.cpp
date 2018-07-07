#include "colorcomponentwidget.h"

#include <functional>

#include <QPainter>
#include <QMouseEvent>

#define BAR_PADDING 4

struct ComponentRecord {
	std::function<qreal(const QColor)> getterFunc;
	std::function<void(qreal, QColor&)> setterFunc;
	std::function<void(qreal, QColor&)> previewSetterFunc;
};

static const ComponentRecord componentRecords[ColorComponentWidget::_ccCount] = {
	// Hsv H
	ComponentRecord{
		[](const QColor &c){return c.hsvHueF();},
		[](qreal val, QColor &c){c.setHsvF(val, c.hsvSaturationF(), c.valueF(), c.alphaF());},
		[](qreal val, QColor &c){c.setHsvF(val, 1, 1);}
	},
	// Hsv S
	ComponentRecord{
		[](const QColor &c){return c.hsvSaturationF();},
		[](qreal val, QColor &c){c.setHsvF(c.hsvHueF(), val, c.valueF(), c.alphaF());},
		[](qreal val, QColor &c){c.setHsvF(c.hsvHueF(), val, 1);}
	},
	// Hsv V
	ComponentRecord{
		[](const QColor &c){return c.valueF();},
		[](qreal val, QColor &c){c.setHsvF(c.hsvHueF(), c.hsvSaturationF(), val, c.alphaF());},
		[](qreal val, QColor &c){c.setHsvF(c.hsvHueF(), 1, val);}
	},

	// Hsl H
	ComponentRecord{
		[](const QColor &c){return c.hslHueF();},
		[](qreal val, QColor &c){c.setHslF(val, c.hslSaturationF(), c.lightnessF(), c.alphaF());},
		[](qreal val, QColor &c){c.setHslF(val, 1, 0.5);}
	},
	// Hsl S
	ComponentRecord{
		[](const QColor &c){return c.hslSaturationF();},
		[](qreal val, QColor &c){c.setHslF(c.hslHueF(), val, c.lightnessF(), c.alphaF());},
		[](qreal val, QColor &c){c.setHslF(c.hslHueF(), val, 0.5);}
	},
	// Hsl L
	ComponentRecord{
		[](const QColor &c){return c.lightnessF();},
		[](qreal val, QColor &c){c.setHslF(c.hslHueF(), c.hslSaturationF(), val, c.alphaF());},
		[](qreal val, QColor &c){c.setHslF(c.hslHueF(), 1, val);},
	},

	// Rgb R
	ComponentRecord{
		[](const QColor &c){return c.redF();},
		[](qreal val, QColor &c){c.setRedF(val);},
		[](qreal val, QColor &c){c.setRgbF(val, c.greenF(), c.blueF());}
	},
	// RGB G
	ComponentRecord{
		[](const QColor &c){return c.greenF();},
		[](qreal val, QColor &c){c.setGreenF(val);},
		[](qreal val, QColor &c){c.setRgbF(c.redF(), val, c.blueF());}
	},
	// RGB B
	ComponentRecord{
		[](const QColor &c){return c.blueF();},
		[](qreal val, QColor &c){c.setBlueF(val);},
		[](qreal val, QColor &c){c.setRgbF(c.redF(), c.greenF(), val);}
	},

	// Alpha
	ComponentRecord{
		[](const QColor &c){return c.alphaF();},
		[](qreal val, QColor &c){c.setAlphaF(val);},
		[](qreal val, QColor &c){c.setAlphaF(val);}
	}
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
	const ComponentRecord &componentRecord = componentRecords[component_];

	// Cannot be because comparison converts to RGB -> in some cases saturation information can be lost
	/*if(color == currentColor_)
		return;*/

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
	QColor c = currentColor_;
	if(component_ != ccAlpha)
		c.setAlphaF(1);

	p.setPen(pen);
	p.setBrush(c);
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

	// Checkerboard pattern
	if(component_ == ccAlpha) {
		static const QColor c1("#ddd"), c2("#aaa");

		p.fillRect(barRect_, c1);
		p.setClipRect(barRect_);
		p.setClipping(true);

		QMatrix m;
		m.scale(4,4);

		p.save();
		p.setWorldMatrix(m, true);
		p.setPen(Qt::NoPen);
		p.setBrush(QBrush(c2, Qt::Dense4Pattern));
		p.drawRect(m.inverted().mapRect(barRect_));
		p.restore();

		p.setClipping(false);
	}

	{
		const qreal progressInc = 1.0 / (qreal) barRect_.width();
		qreal progress = 0;
		QColor color = currentColor_;
		QBrush brush = Qt::transparent;

		QRect currentRect(barRect_.left(), barRect_.top(), 1, barRect_.height());
		while(currentRect.left() <= barRect_.right()) {
			componentRecord.previewSetterFunc(progress, color);
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

	if(!isEnabled())
		return;

	componentRecord.setterFunc(
				qMax<qreal>(0,qMin<qreal>(1,(pos.x() - barRect_.left())/(qreal) barRect_.width())),
				currentColor_
				);
	update();
	emit sigColorChangedByUser(currentColor_);
}
