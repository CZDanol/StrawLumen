#include "presentation_blackscreen.h"

QSharedPointer<Presentation_BlackScreen> Presentation_BlackScreen::create()
{
	return QSharedPointer<Presentation_BlackScreen>(new Presentation_BlackScreen());
}

Presentation_BlackScreen::~Presentation_BlackScreen()
{

}

void Presentation_BlackScreen::drawSlide(QPainter &p, int slideId, const QRect &rect)
{
	Q_UNUSED(p);
	Q_UNUSED(slideId);
	Q_UNUSED(rect);

	// Do. absolutely. nothing.
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

int Presentation_BlackScreen::slideCount() const
{
	return 1;
}

QPixmap Presentation_BlackScreen::slideIdentificationIcon(int) const
{
	static QPixmap icon(":/icons/16/TV Off_16px.png");
	return icon;
}

Presentation_BlackScreen::Presentation_BlackScreen()
{

}
