#include "songsitemmodel.h"

#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QMimeData>
#include <QSqlRecord>

SongsItemModel::SongsItemModel(QObject *parent) : QSqlQueryModel(parent) {
}

Qt::ItemFlags SongsItemModel::flags(const QModelIndex &index) const {
	return QSqlQueryModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

Qt::DropActions SongsItemModel::supportedDragActions() const {
	return Qt::LinkAction;
}

QStringList SongsItemModel::mimeTypes() const {
	static const QStringList result{"application/straw.lumen.song.ids"};
	return result;
}

QMimeData *SongsItemModel::mimeData(const QModelIndexList &indexes) const {
	QMimeData *mimeData = new QMimeData();

	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);

	for(auto &index: indexes) {
		if(index.column() == 1)
			stream << record(index.row()).value("id").toLongLong();
	}

	mimeData->setData("application/straw.lumen.song.ids", data);
	return mimeData;
}

Qt::DropActions SongsItemModel::supportedDropActions() const {
	return Qt::MoveAction | Qt::LinkAction;
}

bool SongsItemModel::canDropMimeData(const QMimeData *data, Qt::DropAction, int, int, const QModelIndex &) const {
	return data->hasFormat("application/straw.lumen.song.null");
}

bool SongsItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
	if(!canDropMimeData(data, action, row, column, parent))
		return false;

	return true;
}
