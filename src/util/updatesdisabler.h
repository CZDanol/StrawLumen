#ifndef UPDATEDISABLER_H
#define UPDATEDISABLER_H

#include <QWidget>

class UpdatesDisabler {

public:
	UpdatesDisabler(QWidget *wgt);
	~UpdatesDisabler();

private:
	QWidget *wgt_;
};

#endif// UPDATEDISABLER_H
