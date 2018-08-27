#ifndef PLAYLISTSDIALOG_H
#define PLAYLISTSDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QStringListModel>
#include <QJsonObject>

namespace Ui {
	class PlaylistsDialog;
}

class PlaylistsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PlaylistsDialog(QWidget *parent = nullptr);
	~PlaylistsDialog();

public:
	void show(bool save);

public slots:
	void saveWorkingPlaylist(qlonglong playlistId);
	bool loadPlaylist(qlonglong playlistId);

	/// Playlist that was last saved/loaded during this program run
	/// Default -1; is also zeroed when the program is cleared
	qlonglong lastTouchPlaylistId() const;
	void clearLastTouchPlaylistId();

private slots:
	void updateUiEnabled();

private slots:
	void requery();
	void on_btnNew_clicked();
	void on_lstPlaylists_itemChanged(QListWidgetItem *item);
	void on_btnRename_clicked();
	void on_lstPlaylists_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
	void on_btnLoad_clicked();
	void on_btnSave_clicked();
	void on_btnClose_clicked();
	void on_btnDelete_clicked();
	void on_lstPlaylists_itemActivated(QListWidgetItem *item);

private:
	Ui::PlaylistsDialog *ui;
	QStringListModel itemsModel_;
	qlonglong lastTouchId_ = -1;

};

PlaylistsDialog *playlistsDialog();

#endif // PLAYLISTSDIALOG_H
