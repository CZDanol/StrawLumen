#include "songsitemmodel.h"

#include <QByteArray>
#include <QDataStream>
#include <QMimeData>
#include <QSqlRecord>

SongsItemModel::SongsItemModel(QObject *parent) : QSqlQueryModel(parent)
{

}

Qt::ItemFlags SongsItemModel::flags(const QModelIndex &index) const
{
	return QSqlQueryModel::flags(index) | Qt::ItemIsDragEnabled;
}

Qt::DropActions SongsItemModel::supportedDragActions() const
{
	return Qt::LinkAction;
}

QStringList SongsItemModel::mimeTypes() const
{
	static const QStringList result {"application/straw.lumen.song.ids"};
	return result;
}

QMimeData *SongsItemModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();

	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);

	for(auto &index : indexes) {
		if(index.column() == 1)
			stream << record(index.row()).value("id").toLongLong();
	}

	mimeData->setData("application/straw.lumen.song.ids", data);
	return mimeData;
}