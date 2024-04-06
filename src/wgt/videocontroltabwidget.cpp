#include "videocontroltabwidget.h"
#include "ui_videocontroltabwidget.h"

#include "presentation/video/videoprojectorwindow.h"

VideoControlTabWidget::VideoControlTabWidget(QWidget *parent) : QWidget(parent),
                                                                ui(new Ui::VideoControlTabWidget) {
	ui->setupUi(this);
	ui->tw->setCornerWidget(ui->twCorner);
}

VideoControlTabWidget::~VideoControlTabWidget() {
	delete ui;
}

void VideoControlTabWidget::setData(bool paused, int videoProgress, int videoLength, int volume, bool muted) {
	ignoreChangeEvents_ = true;
	ui->btnPause->setChecked(paused);

	ui->sldProgress->setMaximum(videoLength);
	ui->sldProgress->setValue(videoProgress);

	ui->lblPosition->setText(QString("%1:%2").arg(videoProgress / 60, 2, 10, QLatin1Char('0')).arg(videoProgress % 60, 2, 10, QLatin1Char('0')));
	ui->lblRemaining->setText(QString("-%1:%2").arg((videoLength - videoProgress) / 60, 2, 10, QLatin1Char('0')).arg((videoLength - videoProgress) % 60, 2, 10, QLatin1Char('0')));

	ui->sldVolume->setValue(muted ? 0 : volume);
	ignoreChangeEvents_ = false;

	static QIcon unmutedButton(":/icons/16/Voice_16px.png");
	static QIcon mutedButton(":/icons/16/Mute_16px.png");
	ui->btnMute->setIcon(muted ? mutedButton : unmutedButton);
}

void VideoControlTabWidget::on_btnPause_clicked(bool checked) {

	videoProjectorWindow->setPaused(checked);
}

void VideoControlTabWidget::on_btnStop_clicked() {
	videoProjectorWindow->stop();
}

void VideoControlTabWidget::on_sldVolume_valueChanged(int value) {
	if(ignoreChangeEvents_)
		return;

	videoProjectorWindow->setVolume(value);
}

void VideoControlTabWidget::on_sldProgress_valueChanged(int value) {
	if(ignoreChangeEvents_)
		return;

	videoProjectorWindow->setPosition(value);
}

void VideoControlTabWidget::on_btnMute_clicked() {
	videoProjectorWindow->setMuted(!videoProjectorWindow->isMuted());
}
