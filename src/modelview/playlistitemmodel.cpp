#include "playlistitemmodel.h"

#include <QIODevice>
#include <QMimeData>

#include "presentation/native/presentation_song.h"
#include "presentation/presentation.h"
#include "rec/playlist.h"

PlaylistItemModel::PlaylistItemModel() {
}

void PlaylistItemModel::setPlaylist(const QSharedPointer<Playlist> &playlist) {
	if(playlist_ == playlist)
		return;

	beginResetModel();

	// Disconnect all signals from the previous playlist
	if(playlist_)
		playlist_->disconnect(this);

	playlist_ = playlist;

	if(playlist_) {
		connect(playlist_.data(), SIGNAL(sigItemsChanged()), this, SLOT(onItemsChanged()));
		connect(playlist_.data(), SIGNAL(sigItemChanged(Presentation *)), this, SLOT(onItemChanged(Presentation *)));
	}

	endResetModel();
}

int PlaylistItemModel::rowCount(const QModelIndex &) const {
	if(playlist_.isNull())
		return 0;

	return playlist_->items().count();
}

int PlaylistItemModel::columnCount(const QModelIndex &) const {
	return 2;
}

QVariant PlaylistItemModel::data(const QModelIndex &index, int role) const {
	if(!index.isValid())
		return QVariant();

	if(playlist_.isNull())
		return QVariant();

	if(index.row() >= playlist_->items().count() || index.row() < 0)
		return QVariant();


	auto item = playlist_->items()[index.row()];
	const auto column = index.column();

	if(role == Qt::DisplayRole) {
		switch(column) {

			case 1:
				return item->identification();

			default:
				return QVariant();
		}
	}
	else if(role == Qt::DecorationRole) {
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

Qt::ItemFlags PlaylistItemModel::flags(const QModelIndex &index) const {
	return {Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | (index.isValid() ? 0 : Qt::ItemIsDropEnabled)};
}

Qt::DropActions PlaylistItemModel::supportedDropActions() const {
	return Qt::MoveAction | Qt::LinkAction;
}

QStringList PlaylistItemModel::mimeTypes() const {
	static const QStringList result{"application/straw.lumen.playlist.items"};
	return result;
}

QMimeData *PlaylistItemModel::mimeData(const QModelIndexList &indexes) const {
	QMimeData *mimeData = new QMimeData();

	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);

	for(auto &index: indexes) {
		if(index.column() == 0)
			stream << index.row();
	}

	mimeData->setData("application/straw.lumen.playlist.items", data);
	return mimeData;
}

bool PlaylistItemModel::canDropMimeData(const QMimeData *mime, Qt::DropAction action, int row, int column, const QModelIndex &parent) const {
	Q_UNUSED(action);
	Q_UNUSED(row);
	Q_UNUSED(column);
	Q_UNUSED(parent);

	return !playlist_.isNull() && (mime->hasFormat("application/straw.lumen.playlist.items") || mime->hasFormat("application/straw.lumen.song.ids"));
}

bool PlaylistItemModel::dropMimeData(const QMimeData *mime, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
	if(!canDropMimeData(mime, action, row, column, parent))
		return false;

	if(action == Qt::IgnoreAction)
		return true;

	if(row == -1)
		row = playlist_->items().size();

	if(mime->hasFormat("application/straw.lumen.playlist.items")) {
		QVector<int> items;
		QDataStream stream(mime->data("application/straw.lumen.playlist.items"));
		while(!stream.atEnd()) {
			int row;
			stream >> row;
			items << row;
		}

		if(items.isEmpty())
			return true;

		int newPos = playlist_->moveItems(items, row);
		emit sigForceSelection(newPos, newPos + items.size() - 1);

		return true;
	}

	if(mime->hasFormat("application/straw.lumen.song.ids")) {
		QDataStream stream(mime->data("application/straw.lumen.song.ids"));
		QVector<QSharedPointer<Presentation>> items;

		while(!stream.atEnd()) {
			qlonglong songId;
			stream >> songId;
			items.append(Presentation_Song::createFromDb(songId));
		}

		playlist_->insertItems(row, items);
		emit sigForceSelection(row, row + items.size() - 1);
		return true;
	}

	return false;
}

void PlaylistItemModel::onItemsChanged() {
	beginResetModel();
	endResetModel();
}

void PlaylistItemModel::onItemChanged(Presentation *item) {
	emit dataChanged(index(item->positionInPlaylist(), 0), index(item->positionInPlaylist(), columnCount(QModelIndex()) - 1));
}
