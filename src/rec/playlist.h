#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QVector>
#include <QSharedPointer>
#include <QMap>
#include <QObject>
#include <QVector>

class Presentation;

class Playlist : public QObject
{
	Q_OBJECT

public:
	Playlist();

public:
	bool addItem(const QSharedPointer<Presentation> &item);
	void deleteItem(const QSharedPointer<Presentation> &item);

	void insertItems(int pos, const QVector<QSharedPointer<Presentation>> &items);

	/// Returns index of the first new item
	int moveItems(const QVector<int> &itemIndexes, int targetPosition);

public:
	const QVector<QSharedPointer<Presentation> > &items() const;
	int slideCount() const;

	QSharedPointer<Presentation> presentationOfSlide(int globalSlideId) const;

signals:
	// Do not emit these signals on your own, rather use emitItems/slides changed
	void sigItemsChanged();
	void sigSlidesChanged();
	void sigItemChanged(Presentation *item); // When item icon/identification/whatever changes

public slots:
	void emitItemsChanged();
	void emitSlidesChanged();

private:
	void updatePlaylistData();

private:
	QVector<QSharedPointer<Presentation>> items_;
	QVector<int> itemOffsets_;
	int slideCount_ = 0;

};

#endif // PLAYLIST_H
