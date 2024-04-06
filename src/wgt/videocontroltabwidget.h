#ifndef VIDEOCONTROLTABWIDGET_H
#define VIDEOCONTROLTABWIDGET_H

#include <QWidget>

namespace Ui {
	class VideoControlTabWidget;
}

class VideoControlTabWidget : public QWidget {
	Q_OBJECT

public:
	explicit VideoControlTabWidget(QWidget *parent = nullptr);
	~VideoControlTabWidget();

public:
	void setData(bool paused, int videoProgress, int videoLength, int volume, bool muted);

private slots:
	void on_btnPause_clicked(bool checked);
	void on_btnStop_clicked();
	void on_sldVolume_valueChanged(int value);
	void on_sldProgress_valueChanged(int value);

	void on_btnMute_clicked();

private:
	Ui::VideoControlTabWidget *ui;
	bool ignoreChangeEvents_ = false;
};

#endif// VIDEOCONTROLTABWIDGET_H
