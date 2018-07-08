#include "presentation_blackscreen.h"

QSharedPointer<Presentation_BlackScreen> Presentation_BlackScreen::create()
{
	return QSharedPointer<Presentation_BlackScreen>(new Presentation_BlackScreen());
}

Presentation_BlackScreen::~Presentation_BlackScreen()
{

}

QString Presentation_BlackScreen::identification() const
{
	return tr("Černá obrazovka");
}

QPixmap Presentation_BlackScreen::icon() const
{
	static QPixmap icon(":/icons/16/TV Off_16px.png");
	return icon;
}

int Presentation_BlackScreen::rawSlideCount() const
{
	return 1;
}

QPixmap Presentation_BlackScreen::rawSlideIdentificationIcon(int) const
{
	static QPixmap icon(":/icons/16/TV Off_16px.png");
	return icon;
}

Presentation_BlackScreen::Presentation_BlackScreen()
{
	initDefaultSlideOrder();
}
