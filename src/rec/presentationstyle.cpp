#include "presentationstyle.h"

#include <QPainter>

PresentationStyle::PresentationStyle(QObject *parent) : QObject(parent)
{

}

void PresentationStyle::setMainTextStyle(const TextStyle &style)
{
	if(mainTextStyle_ == style)
		return;

	mainTextStyle_ = style;
	emit sigChanged();
}

const TextStyle &PresentationStyle::mainTextStyle() const
{
	return mainTextStyle_;
}

void PresentationStyle::drawSlide(QPainter &p, const QRect &rect, const QString &text, const QString &title, const QString &author) const
{

}

void PresentationStyle::operator=(const PresentationStyle &other)
{
	mainTextStyle_ = other.mainTextStyle_;
	emit sigChanged();
}
