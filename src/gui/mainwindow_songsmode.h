#ifndef MAINWINDOW_SONGSMODE_H
#define MAINWINDOW_SONGSMODE_H

#include <QWidget>
#include <QMenu>

namespace Ui {
	class MainWindow_SongsMode;
}

class MainWindow_SongsMode : public QWidget
{
	Q_OBJECT

public:
	explicit MainWindow_SongsMode(QWidget *parent = 0);
	~MainWindow_SongsMode();

public:
	QWidget *menuWidget();

private:
	void setSongEditMode(bool set);
	void updateSongManipulationButtonsEnabled();

private slots:
	void onCurrentSongChanged(qlonglong songId, int prevRowId);
	void onSongListContextMenuRequested(const QPoint &globalPos);

private slots:
	void on_btnNew_clicked();
	void on_btnDiscardChanges_clicked();
	void on_btnSaveChanges_clicked();
	void on_btnEdit_clicked();
	void on_actionDeleteSong_triggered();

private:
	Ui::MainWindow_SongsMode *ui;
	qlonglong currentSongId_ = -1;
	bool isSongEditMode_ = false;

};

#endif // MAINWINDOW_SONGSMODE_H
