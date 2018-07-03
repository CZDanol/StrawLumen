#include "presentation.h"

QString Presentation::rawSlideIdentification(int) const
{
	return QString();
}

QPixmap Presentation::rawSlideIdentificationIcon(int) const
{
	return QPixmap();
}

QString Presentation::rawSlideDescription(int) const
{
	return QString();
}

int Presentation::slideCount() const
{
	return slideOrder_.size();
}

QString Presentation::slideIdentification(int i) const
{
	return i < slideOrder_.size() ? rawSlideIdentification(slideOrder_[i]) : QString();
}

QPixmap Presentation::slideIdentificationIcon(int i) const
{
	return i < slideOrder_.size() ? rawSlideIdentificationIcon(slideOrder_[i]) : QPixmap();
}

QString Presentation::slideDescription(int i) const
{
	return i < slideOrder_.size() ? rawSlideDescription(slideOrder_[i]) : tr("## SNÍMEK NEEXISTUJE ##");
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

QWidget *Presentation::createPropertiesWidget(QWidget *)
{
	return nullptr;
}

void Presentation::initDefaultSlideOrder()
{
	slideOrder_.resize(rawSlideCount());
	for(int i = 0; i < slideOrder_.size(); i ++)
		slideOrder_[i] = i;
}
