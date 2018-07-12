#include "startupsplashscreen.h"
#include "ui_startupsplashscreen.h"

StartupSplashscreen::StartupSplashscreen(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::StartupSplashscreen)
{
	ui->setupUi(this);
	setFixedSize(size());
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	setWindowTitle(QString("Straw Lumen v%1").arg(PROGRAM_VERSION));
}

StartupSplashscreen::~StartupSplashscreen()
{
	delete ui;
}
