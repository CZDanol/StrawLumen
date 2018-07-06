#ifndef COLORCOMPONENTWIDGET_H
#define COLORCOMPONENTWIDGET_H

#include <QWidget>
#include <QPixmap>

class ColorComponentWidget : public QWidget
{
	Q_OBJECT

public:
	enum ColorComponent {
		ccHue, ccSaturation, ccValue, ccLightness,
		ccRed, ccGreen, ccBlue,
		ccAlpha,
		_ccCount
	};

public:
	explicit ColorComponentWidget(QWidget *parent = nullptr);

signals:
	void sigColorChanged(QColor newColor);

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
	void pickColor(QPoint &pos);

private:
	QColor currentColor_;
	QPixmap backgroundCache_;
	ColorComponent component_ = ccHue;
	QRect barRect_;

};

#endif // COLORCOMPONENTWIDGET_H
