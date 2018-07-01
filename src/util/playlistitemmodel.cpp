#include "playlistitemmodel.h"

#include "presentation/playlist.h"
#include "presentation/presentation.h"

PlaylistItemModel::PlaylistItemModel()
{

}

void PlaylistItemModel::setPlaylist(const QSharedPointer<Playlist> &playlist)
{
	if(playlist_ == playlist)
		return;

	beginResetModel();

	// Disconnect all signals from the previous playlist
	if(playlist_)
		playlist_->disconnect(this);

	playlist_ = playlist;

	if(playlist_)
		connect(playlist_.data(), SIGNAL(sigItemsChanged()), this, SLOT(resetModel()));

	endResetModel();
}

int PlaylistItemModel::rowCount(const QModelIndex &) const
{
	if(playlist_.isNull())
		return 0;

	return playlist_->items().count();
}

int PlaylistItemModel::columnCount(const QModelIndex &) const
{
	return 1;
}

QVariant PlaylistItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::DisplayRole)
		return QVariant();

	if(orientation != Qt::Horizontal)
		return QVariant();

	switch(section) {

	case 0:
		return tr("Položka");


	default:
		return QVariant();

	}
}

QVariant PlaylistItemModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if(playlist_.isNull())
		return QVariant();

	if (index.row() >= playlist_->items().count() || index.row() < 0)
		return QVariant();


	auto item = playlist_->items()[index.row()];
	const auto column = index.column();

	if (role == Qt::DisplayRole) {
		switch(column) {

		case 0:
			return item->identification();

		default:
			return QVariant();

		}

	} else if(role == Qt::DecorationRole) {
		return column == 0 ? item->icon() : QVariant();

	}

	return QVariant();
}

void PlaylistItemModel::resetModel()
{
	beginResetModel();
	endResetModel();
}
