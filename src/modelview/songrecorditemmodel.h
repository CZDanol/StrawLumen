#ifndef SONGRECORDITEMMODEL_H
#define SONGRECORDITEMMODEL_H

#include <QAbstractTableModel>
#include <QHash>

#include "job/db.h"

class SongRecordItemModel : public QAbstractTableModel
{
	Q_OBJECT

private:
	struct SongRecord {
		qlonglong songId;
		QString name, author, tags;
	};

public:
	explicit SongRecordItemModel(QObject *parent = nullptr);
	virtual ~SongRecordItemModel();

public:
	void setDb(DBManager *mgr);

signals:
	void sigItemsManipulated(int index, int count);

public:
	QVector<qlonglong> items() const;
	qlonglong itemAt(const QModelIndex &index) const;

	void addItem(qlonglong songId);
	void addItems(const QVector<qlonglong> &songIds);

	void insertItems(int pos, const QVector<qlonglong> &songIds);

	void deleteItem(qlonglong songId);
	void deleteItems(const QVector<qlonglong> songIds);
	void deleteItems(const QModelIndexList &items);

	void clear();

public:
	QVariant data(const QModelIndex &index, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;

	Qt::DropActions supportedDropActions() const override;
	QMimeData *mimeData(const QModelIndexList &indexes) const override;
	bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

private:
	SongRecord *createRecord(qlonglong songId);

	void moveItems(const QVector<int> &itemIndexes, int targetPosition);

private:
	DBManager *db_ = nullptr;
	QVector<SongRecord*> items_;
	QHash<qlonglong, SongRecord*> itemMap_;

};

#endif // SONGRECORDITEMMODEL_H
