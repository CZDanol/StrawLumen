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
	void addItems(const QVector<QSharedPointer<Presentation> > &items);
	void deleteItem(const QSharedPointer<Presentation> &item);

	void insertItems(int pos, const QVector<QSharedPointer<Presentation> > &items);
	void deleteItems(const QVector<QSharedPointer<Presentation> > &items);

	void clear();

	/// Returns index of the first new item
	int moveItems(const QVector<int> &itemIndexes, int targetPosition);

public:
	const QVector<QSharedPointer<Presentation> > &items() const;
	int slideCount() const;

	QSharedPointer<Presentation> presentationOfSlide(int globalSlideId) const;

public:
	QJsonObject toJSON() const;

	/// Clears the playlists and fills it with items from json
	/// Returns true if all items loaded correctly (it may load only some items)
	bool loadFromJSON(const QJsonObject &json);

	static QStringList itemNamesFromJSON(const QJsonObject &json);

	bool areChangesSaved() const;
	void assumeChangesSaved();

signals:
	// Do not emit these signals on your own, rather use emitItems/slides changed
	/// When item order was changed or items were added/removed
	void sigItemsChanged();
	void sigItemsAdded();
	void sigSlidesChanged();

	/// When item icon/identification/whatever changes
	void sigItemChanged(Presentation *item);

	/// When ANYTHING has changed (for saving tracking purposes)
	void sigChanged();

	void sigSaved();
	void sigNameChanged();

public slots:
	void emitItemsChanged();
	void emitSlidesChanged();

public:
	qlonglong dbId;
	QString dbName;

private:
	void updatePlaylistData();

	void connectSignals(const QSharedPointer<Presentation> &p);

private slots:
	void onChanged();
	void onPlaylistRenamed(qlonglong id, const QString &newName);
	void onPresentationMorphedInto(const QSharedPointer<Presentation> &from, const QSharedPointer<Presentation> &to);

private:
	QVector<QSharedPointer<Presentation> > items_;
	QVector<int> itemOffsets_;
	int slideCount_ = 0;
	bool areChangesSaved_ = true;

};

#endif // PLAYLIST_H
