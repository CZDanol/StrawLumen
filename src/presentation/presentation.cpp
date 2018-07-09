#include "presentation.h"

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
