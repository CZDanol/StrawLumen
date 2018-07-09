#include "projectorwindow.h"
#include "ui_projectorwindow.h"

#include "gui/mainwindow.h"
#include "presentation/native/presentationengine_native.h"

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
	p.setRenderHint(QPainter::Antialiasing);
	p.setRenderHint(QPainter::SmoothPixmapTransform);

	p.fillRect(rect(), Qt::black);

	if(presentationEngine_Native->currentPresentation())
		presentationEngine_Native->currentPresentation()->drawSlide(p, presentationEngine_Native->currentSlide(), rect());
}

void ProjectorWindow::changeEvent(QEvent *e)
{
	if(this->isActiveWindow())
		mainWindow->activateWindow();

	QMainWindow::changeEvent(e);
}
