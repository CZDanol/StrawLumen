#include "presentation.h"

int Presentation::slideCount() const
{
	return slideOrder_.size();
}

void Presentation::initDefaultSlideOrder()
{
	slideOrder_.resize(rawSlideCount());
	for(int i = 0; i < slideOrder_.size(); i ++)
		slideOrder_[i] = i;
}
