#ifndef SONGSMODEWINDOW_H
#define SONGSMODEWINDOW_H

#include <QMainWindow>

namespace Ui {
	class SongsModeWindow;
}

class SongsModeWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit SongsModeWindow(QWidget *parent = nullptr);
	~SongsModeWindow();

private:
	Ui::SongsModeWindow *ui;
};

#endif // SONGSMODEWINDOW_H
