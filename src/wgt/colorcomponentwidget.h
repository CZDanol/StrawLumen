#ifndef COLORCOMPONENTWIDGET_H
#define COLORCOMPONENTWIDGET_H

#include <QWidget>
#include <QPixmap>

class ColorComponentWidget : public QWidget
{
	Q_OBJECT

public:
	enum ColorComponent {
		ccHsvHue, ccHsvSaturation, ccValue,
		ccHslHue, ccHslSaturation, ccLightness,
		ccRed, ccGreen, ccBlue,
		ccAlpha,
		_ccCount
	};

public:
	explicit ColorComponentWidget(QWidget *parent = nullptr);

signals:
	void sigColorChangedByUser(QColor newColor);

public:
	void setComponent(ColorComponent component);

public slots:
	void setColor(const QColor &color);

protected:
	void paintEvent(QPaintEvent *) override;
	void mousePressEvent(QMouseEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;

private:
	void updateCache();
	void pickColor(const QPoint &pos);

private:
	QColor currentColor_;
	QPixmap backgroundCache_;
	ColorComponent component_ = ccHsvHue;
	QRect barRect_;

};

#endif // COLORCOMPONENTWIDGET_H
