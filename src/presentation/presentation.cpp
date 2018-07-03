#include "presentation.h"

int Presentation::slideCount() const
{
	return slideOrder_.size();
}

QString Presentation::slideIdentification(int i) const
{
	return slideOrder_.size() > i ? rawSlideIdentification(slideOrder_[i]) : QString();
}

QString Presentation::slideDescription(int i) const
{
	return slideOrder_.size() > i ? rawSlideDescription(slideOrder_[i]) : tr("## SNÍMEK NEEXISTUJE ##");
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

void Presentation::initDefaultSlideOrder()
{
	slideOrder_.resize(rawSlideCount());
	for(int i = 0; i < slideOrder_.size(); i ++)
		slideOrder_[i] = i;
}
