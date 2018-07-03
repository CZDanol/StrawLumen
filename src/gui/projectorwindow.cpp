#include "projectorwindow.h"
#include "ui_projectorwindow.h"

#include "gui/mainwindow.h"

#include <QPainter>
#include <QFocusEvent>

ProjectorWindow *projectorWindow = nullptr;

ProjectorWindow::ProjectorWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::ProjectorWindow)
{
	ui->setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_ShowWithoutActivating);
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

void ProjectorWindow::changeEvent(QEvent *e)
{
	if(this->isActiveWindow())
		mainWindow->activateWindow();
}
