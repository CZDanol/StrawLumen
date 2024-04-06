#ifndef FUSIONPROXYSTYLE_H
#define FUSIONPROXYSTYLE_H

#include <QPainter>
#include <QProxyStyle>
#include <QStyleOptionButton>

class FusionProxyStyle : public QProxyStyle {

public:
	FusionProxyStyle();

public:
	void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

	QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;
};

#endif// FUSIONPROXYSTYLE_H
