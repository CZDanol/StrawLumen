#ifndef MAINWINDOW_PRESENTATIONMODE_H
#define MAINWINDOW_PRESENTATIONMODE_H

#include <QWidget>
#include <QTimer>

#include "util/playlistitemmodel.h"
#include "util/slidesitemmodel.h"
#include "util/slidesitemdelegate.h"

namespace Ui {
	class MainWindow_PresentationMode;
}

class MainWindow_PresentationMode : public QWidget
{
	Q_OBJECT

public:
	explicit MainWindow_PresentationMode(QWidget *parent = 0);
	~MainWindow_PresentationMode();

public:
	QWidget *menuWidget();

protected:
	void dragEnterEvent(QDragEnterEvent *e) override;
	void dropEvent(QDropEvent *e) override;

private slots:
	void onCurrentTimeTimer();
	void onPlaylistForceSelection(int first, int last);
	void onMgrCurrentSlideChanged();
	void onSlideSelected(const QModelIndex &current);

private slots:
	void on_btnEnableProjection_toggled(bool checked);

private:
	Ui::MainWindow_PresentationMode *ui;
	QTimer currentTimeTimer_;

	QSharedPointer<Playlist> playlist_;
	PlaylistItemModel playlistItemModel_;
	SlidesItemModel slidesItemModel_;
	SlidesItemDelegate slidesItemDelegate_;

};

#endif // MAINWINDOW_PRESENTATIONMODE_H
