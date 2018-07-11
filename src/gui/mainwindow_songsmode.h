#ifndef MAINWINDOW_SONGSMODE_H
#define MAINWINDOW_SONGSMODE_H

#include <QWidget>
#include <QMenu>
#include <QCompleter>
#include <QStringList>
#include <QStringListModel>
#include <QRegularExpressionValidator>
#include <QSqlQueryModel>

#include "rec/songsection.h"

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

	/// Returns false if the user does not wish do close the edit mode
	bool askFinishEditMode();

	void updateSongManipulationButtonsEnabled();
	void insertSongSection(const SongSection &section);

private slots:
	void onCurrentSongChanged(qlonglong songId, int prevRowId);
	void onSelectionChanged();
	void onSongListContextMenuRequested(const QPoint &globalPos);

	void fillSongData();

private slots:
	void on_btnNew_clicked();
	void on_btnDiscardChanges_clicked();
	void on_btnSaveChanges_clicked();
	void on_btnEdit_clicked();
	void on_actionDeleteSongs_triggered();
	void on_lnSlideOrder_sigFocused();
	void on_btnInsertChord_clicked();
	void on_btnTransposeUp_clicked();
	void on_btnTransposeDown_clicked();
	void on_btnAddCustomSlideOrderItem_pressed();
	void on_btnInsertSlideSeparator_clicked();
	void on_actionImportOpenSongSong_triggered();
	void on_lnTags_sigFocused();

	void on_btnCreateSongbook_clicked();

private:
	Ui::MainWindow_SongsMode *ui;
	QMenu insertSectionMenu_, importMenu_, songListContextMenu_;

private:
	QRegularExpressionValidator slideOrderValidator_;
	QCompleter *slideOrderCompleter_;
	QStringListModel slideOrderCompleterModel_;
	QMenu addCustomSlideOrderItemMenu_;

private:
	QCompleter *tagsCompleter_;
	QSqlQueryModel tagsCompleterModel_;

private:
	qlonglong currentSongId_ = -1;
	bool isSongEditMode_ = false;

};

#endif // MAINWINDOW_SONGSMODE_H
