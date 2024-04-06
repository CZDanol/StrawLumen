#ifndef SONGSITEMMODEL_H
#define SONGSITEMMODEL_H

#include <QSqlQueryModel>

class SongsItemModel : public QSqlQueryModel {
	Q_OBJECT

public:
	SongsItemModel(QObject *parent = nullptr);

public:
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	Qt::DropActions supportedDragActions() const override;
	QStringList mimeTypes() const override;
	QMimeData *mimeData(const QModelIndexList &indexes) const override;

	Qt::DropActions supportedDropActions() const override;
	bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
};


#endif// SONGSITEMMODEL_H
