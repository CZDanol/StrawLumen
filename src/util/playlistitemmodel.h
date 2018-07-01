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

public:
	void setPlaylist(const QSharedPointer<Playlist> &playlist);

public:
	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	QVariant data(const QModelIndex &index, int role) const override;

private slots:
	void resetModel();

private:
	QSharedPointer<Playlist> playlist_;

};

#endif // PLAYLISTITEMMODEL_H
