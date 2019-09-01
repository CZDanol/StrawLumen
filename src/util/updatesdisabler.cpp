#include "updatesdisabler.h"

UpdatesDisabler::UpdatesDisabler(QWidget *wgt) : wgt_(wgt)
{
	if(wgt)
		wgt->setUpdatesEnabled(false);
}

UpdatesDisabler::~UpdatesDisabler()
{
	if(wgt_)
		wgt_->setUpdatesEnabled(true);
}
