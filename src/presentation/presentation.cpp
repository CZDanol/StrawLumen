#include "presentation.h"

#include <QApplication>

QString Presentation::slideIdentification(int) const
{
	return QString();
}

QPixmap Presentation::slideIdentificationIcon(int) const
{
	return QPixmap();
}

QString Presentation::slideDescription(int) const
{
	return QString();
}


Playlist *Presentation::playlist() const
{
	return playlist_;
}

int Presentation::positionInPlaylist() const
{
	return positionInPlaylist_;
}

int Presentation::globalSlideIdOffset() const
{
	return globalSlideIdOffset_;
}

QPixmap Presentation::specialIcon() const
{
	return QPixmap();
}

QWidget *Presentation::createPropertiesWidget(QWidget *)
{
	return nullptr;
}

void Presentation::deactivatePresentation()
{

}

void Presentation::setSlide(int localSlideId, bool force)
{
	Q_UNUSED(localSlideId);
	Q_UNUSED(force);
}

Presentation::Presentation()
{
	moveToThread(QApplication::instance()->thread());

	connect(this, SIGNAL(sigSlidesChanged()), this, SIGNAL(sigChanged()));
	connect(this, SIGNAL(sigItemChanged(Presentation*)), this, SIGNAL(sigChanged()));
	connect(this, SIGNAL(sigItemChanged(Presentation*)), this, SIGNAL(sigSlidesChanged()));
}
