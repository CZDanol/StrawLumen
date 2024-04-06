#include "videoprojectorwindow.h"
#include "ui_videoprojectorwindow.h"

#include <QAudioOutput>

#include "gui/mainwindow.h"
#include "gui/mainwindow_presentationmode.h"

VideoProjectorWindow *videoProjectorWindow = nullptr;

VideoProjectorWindow::VideoProjectorWindow(QWidget *parent) : QMainWindow(parent),
                                                              ui(new Ui::VideoProjectorWindow) {
	ui->setupUi(this);
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_ShowWithoutActivating);
	setAttribute(Qt::WA_Mapped);

	connect(mainWindow, &MainWindow::sigClosed, this, &VideoProjectorWindow::close);

	videoWgt_ = new QVideoWidget(this);
	ui->wgtStack->addWidget(videoWgt_);

	audioOutput_ = new QAudioOutput(this);
	connect(audioOutput_, &QAudioOutput::mutedChanged, this, &VideoProjectorWindow::updateControlPanel);
	connect(audioOutput_, &QAudioOutput::volumeChanged, this, &VideoProjectorWindow::updateControlPanel);

	mediaPlayer_ = new QMediaPlayer(this);
	mediaPlayer_->setVideoOutput(videoWgt_);
	mediaPlayer_->setAudioOutput(audioOutput_);
	connect(mediaPlayer_, &QMediaPlayer::positionChanged, this, &VideoProjectorWindow::updateControlPanel);
	connect(mediaPlayer_, &QMediaPlayer::mediaStatusChanged, this, &VideoProjectorWindow::updateControlPanel);
}

VideoProjectorWindow::~VideoProjectorWindow() {
	delete ui;
}

void VideoProjectorWindow::playVideo(const QString &filename) {
	setVideo(filename);
	updateBlackScreen();
	mediaPlayer_->play();
}

void VideoProjectorWindow::setVideo(const QString &filename) {
	mediaPlayer_->setSource(QUrl::fromLocalFile(filename));

	updateBlackScreen();
	updateControlPanel();
}

void VideoProjectorWindow::setRepeat(bool set) {
	mediaPlayer_->setLoops(set ? QMediaPlayer::Infinite : 1);
}

void VideoProjectorWindow::setPaused(bool set) {
	updateBlackScreen();

	if(set)
		mediaPlayer_->pause();
	else
		mediaPlayer_->play();
}

void VideoProjectorWindow::setVolume(int volume) {
	audioOutput_->setVolume(volume);
	audioOutput_->setMuted(false);
}

void VideoProjectorWindow::setPosition(int position) {
	mediaPlayer_->setPosition(static_cast<qint64>(position) * 1000);
}

void VideoProjectorWindow::stop() {
	mediaPlayer_->stop();
}

bool VideoProjectorWindow::isMuted() {
	return audioOutput_->isMuted();
}

void VideoProjectorWindow::setMuted(bool set) {
	audioOutput_->setMuted(set);
}

void VideoProjectorWindow::show() {
	QMainWindow::show();
	updateBlackScreen();
}

void VideoProjectorWindow::close() {
	mediaPlayer_->stop();
	QMainWindow::close();
}

void VideoProjectorWindow::setBlackScreen(bool set) {
	isBlackScreen_ = set;
	updateBlackScreen();
}

void VideoProjectorWindow::changeEvent(QEvent *e) {
	if(this->isActiveWindow())
		mainWindow->activateWindow();

	QMainWindow::changeEvent(e);
}

void VideoProjectorWindow::updateControlPanel() {
	mainWindow->presentationMode()->videoControlWidget()->setData(
	  mediaPlayer_->isPlaying(),
	  static_cast<int>(mediaPlayer_->position() / 1000),
	  static_cast<int>(mediaPlayer_->duration() / 1000),
	  audioOutput_->volume(),
	  audioOutput_->isMuted());
}

void VideoProjectorWindow::updateBlackScreen() {
	ui->wgtStack->setCurrentIndex(isBlackScreen_ || !mediaPlayer_->isPlaying() ? 0 : 1);
	repaint();
}
