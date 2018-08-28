#ifndef MAINWINDOW_PRESENTATIONMODE_H
#define MAINWINDOW_PRESENTATIONMODE_H

#include <QWidget>
#include <QTimer>
#include <QMenu>

#include "modelview/playlistitemmodel.h"
#include "modelview/slidesitemmodel.h"
#include "modelview/slidesitemdelegate.h"

namespace Ui {
	class MainWindow_PresentationMode;
}

class PresentationPropertiesWidget;

class MainWindow_PresentationMode : public QWidget
{
	Q_OBJECT

public:
	explicit MainWindow_PresentationMode(QWidget *parent = nullptr);
	~MainWindow_PresentationMode() override;

public:
	QWidget *menuWidget();

	bool askSaveChanges();

public:
	QSharedPointer<Playlist> playlist();

public slots:
	void updatePlaylistsMenu();

private slots:
	void updateControlsUIEnabled();
	void disablePresentation();
	void scrollToCurrentSlideInSlidesView();

private slots:
	void onCurrentTimeTimer();
	void onPlaylistForceSelection(int first, int last);
	void onMgrCurrentSlideChanged(int globalSlideId);
	void onSlideSelected(const QModelIndex &current);
	void onPresentationSelected(const QModelIndex &current);
	void onAfterSlidesViewSlidesChanged();
	void onPlaylistModelReset();
	void onPlaylistContextMenuRequested(const QPoint &point);
	void onSongListContextMenuRequested(const QPoint &point);
	void onSongListItemActivated(qlonglong songId);
	void onAddPresentationMenuAboutToShow();
	void onAddPresentationBeforeMenuAboutToShow();
	void onAddPresentationAfterMenuAboutToShow();
	void onPlaylistManipulatedAnyhow();

private slots:
	void on_btnEnableProjection_clicked(bool checked);
	void on_tvPlaylist_activated(const QModelIndex &index);
	void on_actionDeletePresentation_triggered();
	void on_actionAddBlackScreen_triggered();
	void on_btnSettings_clicked();
	void on_actionAddPowerpointPresentation_triggered();
	void on_actionAddSong_triggered();
	void on_actionEditSong_triggered();
	void on_actionDeleteSongs_triggered();
	void on_actionAddSongsToPlaylist_triggered();
	void on_btnShowHideTwLeftBottom_clicked();
	void on_twLeftBottom_currentChanged(int index);
	void on_actionAddCustomSlidePresentation_triggered();
	void on_actionEditPresentation_triggered();
	void on_actionClearPlaylist_triggered();
	void on_btnPlaylists_pressed();
	void on_actionSavePlaylist_triggered();
	void on_actionLoadPlaylist_triggered();
	void on_actionSavePlaylistAs_triggered();
	void on_actionAddImagesPresentation_triggered();

private:
	Ui::MainWindow_PresentationMode *ui;
	QTimer currentTimeTimer_;
	QMenu playlistContextMenu_, songListContextMenu_, playlistsMenu_, playlistsRecentMenu_;
	bool isTwLeftBottomHidden_ = false;

	/// !=0 -> disabled; DO NOT SET THIS VALUE, ONLY INCREASE/DECREASE
	int disablePlaylistSelectionChange_ = 0;

private:
	QSharedPointer<Presentation> currentPresentation_;
	PresentationPropertiesWidget *presentationPropertiesWidget_ = nullptr;

private:
	QMenu addPresentationMenu_, insertPresentationBeforeMenu_, insertPresentationAfterMenu_;
	std::function<void(const QVector<QSharedPointer<Presentation> >&)> addPresentationsAction_;

private:
	QSharedPointer<Playlist> playlist_;
	PlaylistItemModel playlistItemModel_;
	SlidesItemModel slidesItemModel_;
	SlidesItemDelegate slidesItemDelegate_;

};

#endif // MAINWINDOW_PRESENTATIONMODE_H
