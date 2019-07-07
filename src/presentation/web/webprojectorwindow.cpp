#include "webprojectorwindow.h"
#include "ui_webprojectorwindow.h"

#include <QWebEngineSettings>

#include "gui/mainwindow.h"

WebProjectorWindow *webProjectorWindow = nullptr;

WebProjectorWindow::WebProjectorWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::WebProjectorWindow)
{
	ui->setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_ShowWithoutActivating);
	setAttribute(Qt::WA_Mapped);

	connect(mainWindow, &MainWindow::sigClosed, this, &WebProjectorWindow::close);

	webView_ = new QWebEngineView(this);
	webView_->settings()->setAttribute(QWebEngineSettings::ShowScrollBars, false);
	setCentralWidget(webView_);
}

WebProjectorWindow::~WebProjectorWindow()
{
	delete ui;
}

void WebProjectorWindow::setUrl(const QUrl &url)
{
	webView_->load(url);
	webView_->show();
}

void WebProjectorWindow::setBlackScreen(bool set)
{
	webView_->setVisible(!set);
}

void WebProjectorWindow::changeEvent(QEvent *e)
{
	if(this->isActiveWindow())
		mainWindow->activateWindow();

	QMainWindow::changeEvent(e);
}
