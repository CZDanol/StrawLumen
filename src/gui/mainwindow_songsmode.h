#ifndef MAINWINDOW_SONGSMODE_H
#define MAINWINDOW_SONGSMODE_H

#include <QWidget>
#include <QMenu>
#include <QCompleter>
#include <QStringList>
#include <QStringListModel>

#include "util/songsection.h"

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

public slots:
	void editSong(qlonglong songId);

private:
	void setSongEditMode(bool set);
	void updateSongManipulationButtonsEnabled();
	void insertSongSection(const SongSection &section);

private slots:
	void onCurrentSongChanged(qlonglong songId, int prevRowId);
	void onSongListContextMenuRequested(const QPoint &globalPos);

private slots:
	void on_btnNew_clicked();
	void on_btnDiscardChanges_clicked();
	void on_btnSaveChanges_clicked();
	void on_btnEdit_clicked();
	void on_actionDeleteSong_triggered();
	void on_lnSlideOrder_sigFocused();

private:
	Ui::MainWindow_SongsMode *ui;
	QMenu *insertSectionMenu;

private:
	QCompleter slideOrderCompleter_;
	QStringListModel slideOrderCompleterModel_;

private:
	qlonglong currentSongId_ = -1;
	bool isSongEditMode_ = false;

};

#endif // MAINWINDOW_SONGSMODE_H