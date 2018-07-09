#ifndef SONGSITEMMODEL_H
#define SONGSITEMMODEL_H

#include <QSqlQueryModel>

class SongsItemModel : public QSqlQueryModel
{
	Q_OBJECT

public:
	SongsItemModel(QObject *parent = nullptr);

public:
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	Qt::DropActions supportedDragActions() const override;
	QStringList mimeTypes() const override;
	QMimeData *mimeData(const QModelIndexList &indexes) const override;

};

#endif // SONGSITEMMODEL_H
