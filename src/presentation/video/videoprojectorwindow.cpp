#include "videoprojectorwindow.h"
#include "ui_videoprojectorwindow.h"

#include "gui/mainwindow.h"

VideoProjectorWindow *videoProjectorWindow = nullptr;

VideoProjectorWindow::VideoProjectorWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::VideoProjectorWindow)
{
	ui->setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_ShowWithoutActivating);
	setAttribute(Qt::WA_Mapped);

	connect(mainWindow, &MainWindow::sigClosed, this, &VideoProjectorWindow::close);

	videoWgt_ = new QVideoWidget(this);
	setCentralWidget(videoWgt_);

	mediaPlayer_ = new QMediaPlayer(this);
	mediaPlayer_->setVideoOutput(videoWgt_);
}

VideoProjectorWindow::~VideoProjectorWindow()
{
	delete ui;
}

void VideoProjectorWindow::playVideo(const QString &filename)
{
	mediaPlayer_->setMedia(QUrl::fromLocalFile(filename));
	mediaPlayer_->play();
}

void VideoProjectorWindow::close()
{
	mediaPlayer_->stop();
	QMainWindow::close();
}

void VideoProjectorWindow::changeEvent(QEvent *e)
{
	if(this->isActiveWindow())
		mainWindow->activateWindow();

	QMainWindow::changeEvent(e);
}
