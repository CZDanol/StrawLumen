#ifndef SLIDESITEMMODEL_H
#define SLIDESITEMMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QSharedPointer>

class Playlist;
class Presentation;

class SlidesItemModel : public QAbstractTableModel {
	Q_OBJECT

public:
	enum class Column : int {
		presentation,
		id,
		text
	};

	enum class UserData : int {
		isLastInPresentation = Qt::UserRole,
		customIcon
	};

public:
	SlidesItemModel();

signals:
	void sigAfterSlidesChanged();

public:
	void setPlaylist(const QSharedPointer<Playlist> &playlist);

public:
	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	QVariant data(const QModelIndex &index, int role) const override;

private slots:
	void onSlidesChanged();

private:
	QSharedPointer<Playlist> playlist_;
};

#endif// SLIDESITEMMODEL_H
