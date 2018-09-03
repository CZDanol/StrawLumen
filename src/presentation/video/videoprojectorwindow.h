#ifndef VIDEOPROJECTORWINDOW_H
#define VIDEOPROJECTORWINDOW_H

#include <QMainWindow>
#include <QVideoWidget>
#include <QMediaPlayer>

namespace Ui {
	class VideoProjectorWindow;
}

class VideoProjectorWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit VideoProjectorWindow(QWidget *parent = nullptr);
	~VideoProjectorWindow() override;

public:
	void playVideo(const QString &filename);
	void close();

protected:
	void changeEvent(QEvent *) override;

private:
	Ui::VideoProjectorWindow *ui;
	QVideoWidget *videoWgt_;
	QMediaPlayer *mediaPlayer_;

};

extern VideoProjectorWindow *videoProjectorWindow;

#endif // VIDEOPROJECTORWINDOW_H
