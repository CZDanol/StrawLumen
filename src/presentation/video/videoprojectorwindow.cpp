#include "videoprojectorwindow.h"
#include "ui_videoprojectorwindow.h"

#include "gui/mainwindow.h"
#include "gui/mainwindow_presentationmode.h"

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
	ui->wgtStack->addWidget(videoWgt_);

	playlist_ = new QMediaPlaylist(this);

	mediaPlayer_ = new QMediaPlayer(this);
	mediaPlayer_->setVideoOutput(videoWgt_);
	mediaPlayer_->setPlaylist(playlist_);
	connect(mediaPlayer_, &QMediaPlayer::positionChanged, this, &VideoProjectorWindow::updateControlPanel);
	connect(mediaPlayer_, &QMediaPlayer::stateChanged, this, &VideoProjectorWindow::updateControlPanel);
	connect(mediaPlayer_, &QMediaPlayer::volumeChanged, this, &VideoProjectorWindow::updateControlPanel);
	connect(mediaPlayer_, &QMediaPlayer::mutedChanged, this, &VideoProjectorWindow::updateControlPanel);
}

VideoProjectorWindow::~VideoProjectorWindow()
{
	delete ui;
}

void VideoProjectorWindow::playVideo(const QString &filename)
{
	setVideo(filename);
	updateBlackScreen();
	mediaPlayer_->play();
}

void VideoProjectorWindow::setVideo(const QString &filename)
{
	playlist_->clear();
	playlist_->addMedia(QUrl::fromLocalFile(filename));

	updateBlackScreen();
	updateControlPanel();
}

void VideoProjectorWindow::setRepeat(bool set)
{
	playlist_->setPlaybackMode(set ? QMediaPlaylist::Loop : QMediaPlaylist::Sequential);
}

void VideoProjectorWindow::setPaused(bool set)
{
	updateBlackScreen();

	if(set)
		mediaPlayer_->pause();
	else
		mediaPlayer_->play();
}

void VideoProjectorWindow::setVolume(int volume)
{
	mediaPlayer_->setVolume(volume);
	mediaPlayer_->setMuted(false);
}

void VideoProjectorWindow::setPosition(int position)
{
	mediaPlayer_->setPosition(static_cast<qint64>(position) * 1000);
}

void VideoProjectorWindow::stop()
{
	mediaPlayer_->stop();
}

bool VideoProjectorWindow::isMuted()
{
	return mediaPlayer_->isMuted();
}

void VideoProjectorWindow::setMuted(bool set)
{
	mediaPlayer_->setMuted(set);
}

void VideoProjectorWindow::show()
{
	QMainWindow::show();
	updateBlackScreen();
}

void VideoProjectorWindow::close()
{
	mediaPlayer_->stop();
	QMainWindow::close();
}

void VideoProjectorWindow::setBlackScreen(bool set)
{
	isBlackScreen_ = set;
	updateBlackScreen();
}

void VideoProjectorWindow::changeEvent(QEvent *e)
{
	if(this->isActiveWindow())
		mainWindow->activateWindow();

	QMainWindow::changeEvent(e);
}

void VideoProjectorWindow::updateControlPanel()
{
	mainWindow->presentationMode()->videoControlWidget()->setData(
				mediaPlayer_->state() != QMediaPlayer::PlayingState,
				static_cast<int>(mediaPlayer_->position() / 1000),
				static_cast<int>(mediaPlayer_->duration() / 1000),
				mediaPlayer_->volume(),
				mediaPlayer_->isMuted());
}

void VideoProjectorWindow::updateBlackScreen()
{
	ui->wgtStack->setCurrentIndex(isBlackScreen_ || mediaPlayer_->state() == QMediaPlayer::StoppedState ? 0 : 1);
	repaint();
}
