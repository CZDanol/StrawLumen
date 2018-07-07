#include "presentationstyle.h"

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
