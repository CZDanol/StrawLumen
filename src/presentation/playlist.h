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

public:
	inline const QVector<QSharedPointer<Presentation> > &items() const {
		return items_;
	}

	inline int slideCount() const {
		return slideCount_;
	}

	QSharedPointer<Presentation> presentationOfSlide(int slide) const;

public:
	/// Returns index of the first new item
	int moveItems(const QVector<int> &itemIndexes, int targetPosition);

signals:
	void sigItemsChanged();
	void sigSlidesChanged();

private slots:
	void onItemsChanged();

private:
	QVector<QSharedPointer<Presentation>> items_;
	QVector<int> itemOffsets_;
	int slideCount_ = 0;

};

#endif // PLAYLIST_H
