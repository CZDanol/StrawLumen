#include "playlistitemmodel.h"

#include <QMimeData>

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

	if(playlist_) {
		connect(playlist_.data(), SIGNAL(sigItemsChanged()), this, SLOT(onItemsChanged()));
		connect(playlist_.data(), SIGNAL(sigItemChanged(Presentation*)), this, SLOT(onItemChanged(Presentation*)));
	}

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
	return 2;
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

		case 1:
			return item->identification();

		default:
			return QVariant();

		}

	} else if(role == Qt::DecorationRole) {
		switch(column) {

		case 0:
			return item->icon();

		case 1:
			return item->specialIcon();

		default:
			return QVariant();

		}
	}

	return QVariant();
}

Qt::ItemFlags PlaylistItemModel::flags(const QModelIndex &index) const
{
	return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | ( index.isValid() ? 0 : Qt::ItemIsDropEnabled );
}

Qt::DropActions PlaylistItemModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

QStringList PlaylistItemModel::mimeTypes() const
{
	static const QStringList result {"application/straw.lumen.playlist.items"};
	return result;
}

QMimeData *PlaylistItemModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();

	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);

	for(auto &index : indexes) {
		if(index.column() == 0)
			stream << index.row();
	}

	mimeData->setData("application/straw.lumen.playlist.items", data);
	return mimeData;
}

bool PlaylistItemModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
	Q_UNUSED(action);
	Q_UNUSED(row);
	Q_UNUSED(column);
	Q_UNUSED(parent);

	return !playlist_.isNull() && data->hasFormat("application/straw.lumen.playlist.items");
}

bool PlaylistItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	if(!canDropMimeData(data, action, row, column, parent))
		return false;

	if(action == Qt::IgnoreAction)
		return true;

	QVector<int> items;
	QDataStream stream(data->data("application/straw.lumen.playlist.items"));
	while(!stream.atEnd()) {
		int row;
		stream >> row;
		items << row;
	}

	if(items.isEmpty())
		return true;

	if(row == -1)
		row = playlist_->items().size();

	int newPos = playlist_->moveItems(items, row);
	emit sigForceSelection(newPos, newPos + items.size() - 1);

	return true;
}

void PlaylistItemModel::onItemsChanged()
{
	beginResetModel();
	endResetModel();
}

void PlaylistItemModel::onItemChanged(Presentation *item)
{
	emit dataChanged(index(item->positionInPlaylist(), 0), index(item->positionInPlaylist(), columnCount(QModelIndex())-1));
}
