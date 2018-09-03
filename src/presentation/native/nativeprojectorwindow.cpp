#include "nativeprojectorwindow.h"
#include "ui_nativeprojectorwindow.h"

#include "gui/mainwindow.h"
#include "presentation/native/presentationengine_native.h"

#include <QPainter>
#include <QFocusEvent>
#include <QLayout>

NativeProjectorWindow *nativeProjectorWindow = nullptr;

NativeProjectorWindow::NativeProjectorWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::NativeProjectorWindow)
{
	ui->setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_ShowWithoutActivating);
	setAttribute(Qt::WA_Mapped);

	connect(mainWindow, &MainWindow::sigClosed, this, &NativeProjectorWindow::close);
}

NativeProjectorWindow::~NativeProjectorWindow()
{
	delete ui;
}

void NativeProjectorWindow::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);
	p.setRenderHint(QPainter::SmoothPixmapTransform);

	p.fillRect(rect(), Qt::black);

	if(!presentationEngine_Native->isBlackScreen() && presentationEngine_Native->currentPresentation())
		presentationEngine_Native->currentPresentation()->drawSlide(p, presentationEngine_Native->currentSlide(), rect());
}

void NativeProjectorWindow::changeEvent(QEvent *e)
{
	if(this->isActiveWindow())
		mainWindow->activateWindow();

	QMainWindow::changeEvent(e);
}
