#include "nativeprojectorwindow.h"
#include "ui_nativeprojectorwindow.h"

#include "gui/mainwindow.h"
#include "presentation/native/presentationengine_native.h"

#include <QFocusEvent>
#include <QLayout>
#include <QPainter>

NativeProjectorWindow *nativeProjectorWindow = nullptr;

NativeProjectorWindow::NativeProjectorWindow(QWidget *parent) : QMainWindow(parent),
                                                                ui(new Ui::NativeProjectorWindow) {
	ui->setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_ShowWithoutActivating);
	setAttribute(Qt::WA_Mapped);

	installEventFilter(mainWindow);

	connect(mainWindow, &MainWindow::sigClosed, this, &NativeProjectorWindow::close);
}

NativeProjectorWindow::~NativeProjectorWindow() {
	delete ui;
}

void NativeProjectorWindow::paintEvent(QPaintEvent *) {
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);
	p.setRenderHint(QPainter::SmoothPixmapTransform);

	p.fillRect(rect(), Qt::black);

	if(!presentationEngine_native->isBlackScreen() && presentationEngine_native->currentPresentation())
		presentationEngine_native->currentPresentation()->drawSlide(p, presentationEngine_native->currentSlide(), rect());
}

void NativeProjectorWindow::changeEvent(QEvent *e) {
	if(this->isActiveWindow())
		mainWindow->activateWindow();

	QMainWindow::changeEvent(e);
}
