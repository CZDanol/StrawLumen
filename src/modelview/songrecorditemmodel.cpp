#include "songrecorditemmodel.h"

#include <QDataStream>
#include <QByteArray>
#include <QMimeData>

SongRecordItemModel::SongRecordItemModel(QObject *parent) : QAbstractTableModel(parent)
{
	setDb(db);
}

SongRecordItemModel::~SongRecordItemModel()
{
	qDeleteAll(items_);
}

void SongRecordItemModel::setDb(DBManager *mgr)
{
	db_ = mgr;
	clear();
}

QVector<qlonglong> SongRecordItemModel::items() const
{
	QVector<qlonglong> result(items_.size());
	for(int i = 0; i < items_.size(); i++)
		result[i] = items_[i]->songId;

	return result;
}

qlonglong SongRecordItemModel::itemAt(const QModelIndex &index) const
{
	if(index.row() < 0 || index.row() >= items_.size())
		return -1;

	return items_[index.row()]->songId;
}

void SongRecordItemModel::addItem(qlonglong songId)
{
	if(itemMap_.contains(songId))
		return;

	SongRecord *rec = createRecord(songId);

	beginInsertRows(QModelIndex(), items_.size(), items_.size());
	items_.append(rec);
	itemMap_.insert(songId, rec);
	endInsertRows();

	emit sigItemsManipulated(items_.size()-1, 1);
}

void SongRecordItemModel::addItems(const QVector<qlonglong> &songIds)
{
	if(!songIds.size())
		return;

	QVector<SongRecord*> records;
	records.reserve(songIds.size());

	for(qlonglong songId : songIds) {
		if(itemMap_.contains(songId))
			continue;

		SongRecord *rec = createRecord(songId);

		records.append(rec);
		itemMap_.insert(rec->songId, rec);
	}

	if(!records.size())
		return;

	beginInsertRows(QModelIndex(), items_.size(), items_.size() + records.size() - 1);
	items_.append(records);
	endInsertRows();

	emit sigItemsManipulated(items_.size()-records.size(), records.size());
}

void SongRecordItemModel::insertItems(int pos, const QVector<qlonglong> &songIds)
{
	if(!songIds.size())
		return;

	if(pos < 0 || pos > items_.size())
		pos = items_.size();

	QList<SongRecord*> records;
	records.reserve(songIds.size());

	for(qlonglong songId : songIds) {
		if(itemMap_.contains(songId))
			continue;

		SongRecord *rec = createRecord(songId);

		records.append(rec);
		itemMap_.insert(rec->songId, rec);
	}

	const int origSize = items_.size();
	const int cnt = records.size();

	if(!cnt)
		return;

	beginInsertRows(QModelIndex(), pos, pos + cnt - 1);

	items_.resize(origSize + records.size());

	for(int i = origSize-1; i >= pos; i--)
		items_[i+cnt] = items_[i];

	for(int i = pos; i < pos+cnt; i++)
		items_[i] = records[i-pos];

	endInsertRows();

	emit sigItemsManipulated(pos, cnt);
}

void SongRecordItemModel::deleteItem(qlonglong songId)
{
	SongRecord *rec = itemMap_.value(songId, nullptr);
	if(!rec)
		return;

	int recPos = items_.indexOf(rec);

	beginRemoveRows(QModelIndex(), recPos, recPos);
	items_.removeAt(recPos);
	itemMap_.remove(songId);
	endRemoveRows();

	delete rec;
}

void SongRecordItemModel::deleteItems(const QVector<qlonglong> songIds)
{
	for(const qlonglong songId : songIds)
		deleteItem(songId);
}

void SongRecordItemModel::deleteItems(const QModelIndexList &items)
{
	QVector<qlonglong> ids;
	ids.reserve(items.size());

	for(const QModelIndex &index : items)
		ids.append(itemAt(index));

	deleteItems(ids);
}

void SongRecordItemModel::clear()
{
	beginResetModel();
	items_.clear();
	itemMap_.clear();
	endResetModel();
}

QVariant SongRecordItemModel::data(const QModelIndex &index, int role) const
{
	if(role != Qt::DisplayRole)
		return QVariant();

	SongRecord *rec = items_[index.row()];

	switch(index.column()) {

	case 0:
		return rec->name;

	case 1:
		return rec->author;

	case 2:
		return rec->tags;

	default:
		return QVariant();

	}
}

Qt::ItemFlags SongRecordItemModel::flags(const QModelIndex &index) const
{
	if(index.isValid())
		return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
	else
		return Qt::ItemIsDropEnabled;
}

QVariant SongRecordItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::DisplayRole || orientation != Qt::Horizontal)
		return QVariant();

	switch(section) {

	case 0:
		return tr("Název");

	case 1:
		return tr("Autor");

	case 2:
			return tr("Štítky");

	default:
		return QVariant();

	}
}

int SongRecordItemModel::rowCount(const QModelIndex &) const
{
	return items_.size();
}

int SongRecordItemModel::columnCount(const QModelIndex &) const
{
	return 3;
}

Qt::DropActions SongRecordItemModel::supportedDropActions() const
{
	return Qt::MoveAction | Qt::LinkAction;
}

QMimeData *SongRecordItemModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();

	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);

	for(auto &index : indexes) {
		if(index.column() == 0)
			stream << index.row();
	}

	mimeData->setData("application/straw.lumen.songRecordList.items", data);
	return mimeData;
}

bool SongRecordItemModel::canDropMimeData(const QMimeData *mime, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
	Q_UNUSED(action);
	Q_UNUSED(row);
	Q_UNUSED(column);
	Q_UNUSED(parent);

	return
			mime->hasFormat("application/straw.lumen.songRecordList.items")
			|| mime->hasFormat("application/straw.lumen.song.ids")
			;
}

bool SongRecordItemModel::dropMimeData(const QMimeData *mime, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	if(!canDropMimeData(mime, action, row, column, parent))
		return false;

	if(action == Qt::IgnoreAction)
		return true;

	if(row == -1)
		row = items_.size();

	if(mime->hasFormat("application/straw.lumen.songRecordList.items")) {
		QVector<int> items;
		QDataStream stream(mime->data("application/straw.lumen.songRecordList.items"));
		while(!stream.atEnd()) {
			int row;
			stream >> row;
			items << row;
		}

		if(items.isEmpty())
			return true;

		moveItems(items, row);

		return true;
	}

	if(mime->hasFormat("application/straw.lumen.song.ids")) {
		QDataStream stream(mime->data("application/straw.lumen.song.ids"));
		QVector<qlonglong> items;

		while(!stream.atEnd()) {
			qlonglong songId;
			stream >> songId;
			items << songId;
		}

		insertItems(row, items);
		return true;
	}

	return false;
}

SongRecordItemModel::SongRecord *SongRecordItemModel::createRecord(qlonglong songId)
{
	QSqlRecord r;
	if(db_)
		r = db_->selectRow("SELECT name, author, (SELECT GROUP_CONCAT(tag) FROM song_tags WHERE song = songs.id ORDER BY tag ASC) as tags FROM songs WHERE id = ?", {songId});

	SongRecord *rec = new SongRecord();
	rec->songId = songId;
	rec->name = r.value("name").toString();
	rec->author = r.value("author").toString();
	rec->tags = r.value("tags").toString();

	return rec;
}

void SongRecordItemModel::moveItems(const QVector<int> &itemIndexes, int targetPosition)
{
	if(itemIndexes.isEmpty())
		return;

	QVector<SongRecord*> movedItems(itemIndexes.size());

	int adjustedTargetPosition = targetPosition;

	for(int i = 0; i < movedItems.size(); i ++) {
		const int index = itemIndexes[i];
		movedItems[i] = items_[index];

		if(index < targetPosition)
			adjustedTargetPosition--;
	}

	const int finalTargetPosition = adjustedTargetPosition;

	beginResetModel();

	for(auto &movedItem : movedItems)
		items_.removeOne(movedItem);

	for(auto &movedItem : movedItems)
		items_.insert(adjustedTargetPosition++, movedItem);

	endResetModel();
	emit sigItemsManipulated(finalTargetPosition, movedItems.size());
}
