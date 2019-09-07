#include "songsmodewindow.h"
#include "ui_songsmodewindow.h"

SongsModeWindow::SongsModeWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::SongsModeWindow)
{
	ui->setupUi(this);
}

SongsModeWindow::~SongsModeWindow()
{
	delete ui;
}
