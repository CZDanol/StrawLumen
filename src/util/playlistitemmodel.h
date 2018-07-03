#ifndef PLAYLISTITEMMODEL_H
#define PLAYLISTITEMMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QSharedPointer>

class Playlist;
class Presentation;

class PlaylistItemModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	PlaylistItemModel();

signals:
	void sigForceSelection(int first, int last);

public:
	void setPlaylist(const QSharedPointer<Playlist> &playlist);

public:
	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	Qt::DropActions supportedDropActions() const override;
	QStringList mimeTypes() const override;
	QMimeData *mimeData(const QModelIndexList &indexes) const override;
	bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

private slots:
	void onItemsChanged();

private:
	QSharedPointer<Playlist> playlist_;
	int prevRowCount_ = 0;

};

#endif // PLAYLISTITEMMODEL_H
