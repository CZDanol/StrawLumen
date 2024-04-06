#ifndef VIDEOPROJECTORWINDOW_H
#define VIDEOPROJECTORWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>

namespace Ui {
	class VideoProjectorWindow;
}

class VideoProjectorWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit VideoProjectorWindow(QWidget *parent = nullptr);
	~VideoProjectorWindow() override;

public:
	void playVideo(const QString &filename);
	void setVideo(const QString &filename);

	void setRepeat(bool set);
	void setPaused(bool set);
	void setVolume(int volume);
	void setPosition(int position);
	void stop();

	bool isMuted();
	void setMuted(bool set);

	void show();
	void close();
	void setBlackScreen(bool set);

protected:
	void changeEvent(QEvent *) override;

private slots:
	void updateControlPanel();
	void updateBlackScreen();

private:
	Ui::VideoProjectorWindow *ui;
	bool isBlackScreen_ = false;
	QVideoWidget *videoWgt_;
	QAudioOutput *audioOutput_;
	QMediaPlayer *mediaPlayer_;
};

extern VideoProjectorWindow *videoProjectorWindow;

#endif// VIDEOPROJECTORWINDOW_H
