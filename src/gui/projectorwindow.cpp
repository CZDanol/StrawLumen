#include "projectorwindow.h"
#include "ui_projectorwindow.h"

#include <QPainter>

ProjectorWindow::ProjectorWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::ProjectorWindow)
{
	ui->setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
}

ProjectorWindow::~ProjectorWindow()
{
	delete ui;
}

void ProjectorWindow::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::black);
}
