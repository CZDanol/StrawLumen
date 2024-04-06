#ifndef PLAYLISTSDIALOG_H
#define PLAYLISTSDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QListWidgetItem>
#include <QStringListModel>

namespace Ui {
	class PlaylistsDialog;
}

class PlaylistsDialog : public QDialog {
	Q_OBJECT

public:
	explicit PlaylistsDialog(QWidget *parent = nullptr);
	~PlaylistsDialog();

public:
	void show(bool save);

public slots:
	void saveWorkingPlaylist(qlonglong playlistId);
	bool loadPlaylist(qlonglong playlistId);

private slots:
	void updateUiEnabled();
	void requery();

private slots:
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
};

PlaylistsDialog *playlistsDialog();

#endif// PLAYLISTSDIALOG_H
