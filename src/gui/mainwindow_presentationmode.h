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
	explicit MainWindow_PresentationMode(QWidget *parent = 0);
	~MainWindow_PresentationMode();

public:
	QWidget *menuWidget();

public:
	QSharedPointer<Playlist> playlist();

protected:
	void dragEnterEvent(QDragEnterEvent *e) override;
	void dropEvent(QDropEvent *e) override;

private slots:
	void updateControlsUIEnabled();
	void disablePresentation();

private slots:
	void onCurrentTimeTimer();
	void onPlaylistForceSelection(int first, int last);
	void onMgrCurrentSlideChanged(int globalSlideId);
	void onSlideSelected(const QModelIndex &current);
	void onPresentationSelected(const QModelIndex &current);
	void onAfterSlidesViewSlidesChanged();
	void onPlaylistModelReset();
	void onPlaylistContextMenuRequested(const QPoint &point);
	void onSongListItemActivated(qlonglong songId);

private slots:
	void on_btnEnableProjection_clicked(bool checked);
	void on_tvPlaylist_activated(const QModelIndex &index);
	void on_actionDeletePresentation_triggered();
	void on_actionAddBlackScreen_triggered();
	void on_btnSettings_clicked();
	void on_actionAddPowerpointPresentation_triggered();
	void on_actionAddSong_triggered();

private:
	Ui::MainWindow_PresentationMode *ui;
	PresentationPropertiesWidget *presentationPropertiesWidget_ = nullptr;
	QTimer currentTimeTimer_;
	QMenu playlistContextMenu_, addPresentationMenu_;

private:
	QSharedPointer<Playlist> playlist_;
	PlaylistItemModel playlistItemModel_;
	SlidesItemModel slidesItemModel_;
	SlidesItemDelegate slidesItemDelegate_;

};

#endif // MAINWINDOW_PRESENTATIONMODE_H
