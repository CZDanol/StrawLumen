#include "simpleupdater.h"
#include "ui_simpleupdater.h"

#include "gui/mainwindow.h"

SimpleUpdater::SimpleUpdater(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SimpleUpdater)
{
	ui->setupUi(this);
}

SimpleUpdater::~SimpleUpdater()
{
	delete ui;
}

SimpleUpdater *simpleUpdater()
{
	static SimpleUpdater *dlg = nullptr;
	if(!dlg)
		dlg = new SimpleUpdater(mainWindow);

	return dlg;
}
