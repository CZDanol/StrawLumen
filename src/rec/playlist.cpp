#include "playlist.h"

#include "presentation/presentation.h"

Playlist::Playlist()
{

}

bool Playlist::addItem(const QSharedPointer<Presentation> &item)
{
	if(item.isNull())
		return false;

	Q_ASSERT(!item->playlist_);

	item->playlist_ = this;

	items_.append(item);
	connect(item.data(), SIGNAL(sigSlidesChanged()), this, SLOT(emitSlidesChanged()));
	connect(item.data(), SIGNAL(sigItemChanged(Presentation*)), this, SIGNAL(sigItemChanged(Presentation*)));

	emitItemsChanged();
	return true;
}

void Playlist::addItems(const QList<QSharedPointer<Presentation> > &items)
{
	items_.reserve(items.size());

	for(const QSharedPointer<Presentation> &item : items) {
		if(item.isNull())
			continue;

		Q_ASSERT(!item->playlist_);

		item->playlist_ = this;

		items_.append(item);
		connect(item.data(), SIGNAL(sigSlidesChanged()), this, SLOT(emitSlidesChanged()));
		connect(item.data(), SIGNAL(sigItemChanged(Presentation*)), this, SIGNAL(sigItemChanged(Presentation*)));
	}

	emitItemsChanged();
}

int Playlist::moveItems(const QVector<int> &itemIndexes, int targetPosition)
{
	if(itemIndexes.isEmpty())
		return -1;

	QVector<QSharedPointer<Presentation>> movedItems;
	movedItems.resize(itemIndexes.size());

	int adjustedTargetPosition = targetPosition;

	for(int i = 0; i < movedItems.size(); i ++) {
		const int index = itemIndexes[i];
		movedItems[i] = items_[index];

		if(index < targetPosition)
			adjustedTargetPosition--;
	}

	for(auto &movedItem : movedItems)
		items_.removeOne(movedItem);

	for(auto &movedItem : movedItems)
		items_.insert(adjustedTargetPosition++, movedItem);

	for(int i = 0; i < items_.size(); i++)
		items_[i]->positionInPlaylist_ = i;

	emitItemsChanged();

	return movedItems[0]->positionInPlaylist_;
}

void Playlist::deleteItem(const QSharedPointer<Presentation> &item)
{
	if(!item || item->playlist() != this)
		return;

	item->playlist_ = nullptr;
	items_.remove(item->positionInPlaylist_);

	emitItemsChanged();
}

void Playlist::insertItems(int pos, const QVector<QSharedPointer<Presentation> > &items)
{
	if(pos > items_.count() || pos < -1)
		pos = items_.count() - 1;

	items_.insert(pos, items.count() - items.count(nullptr), nullptr);

	for(const QSharedPointer<Presentation> &item : items) {
		if(item.isNull())
			continue;

		Q_ASSERT(!item->playlist_);

		item->playlist_ = this;

		items_[pos++] = item;
		connect(item.data(), SIGNAL(sigSlidesChanged()), this, SLOT(emitSlidesChanged()));
		connect(item.data(), SIGNAL(sigItemChanged(Presentation*)), this, SIGNAL(sigItemChanged(Presentation*)));
	}

	emitItemsChanged();
}

void Playlist::deleteItems(const QVector<QSharedPointer<Presentation> > &items)
{
	for(const QSharedPointer<Presentation> &item : items) {
		if(!item || item->playlist() != this)
			continue;

		item->playlist_ = nullptr;
		items_.removeOne(item);
	}

	emitItemsChanged();
}

const QVector<QSharedPointer<Presentation> > &Playlist::items() const
{
	return items_;
}

int Playlist::slideCount() const
{
	return slideCount_;
}

QSharedPointer<Presentation> Playlist::presentationOfSlide(int globalSlideId) const
{
	if(globalSlideId < 0 || globalSlideId >= slideCount_)
		return nullptr;

	// Simple binary search
	int lower = 0, upper = itemOffsets_.size();
	while(lower < upper-1) {
		int current = (lower + upper) / 2;
		int val = items_[current]->globalSlideIdOffset();

		if(globalSlideId >= val)
			lower = current;
		else
			upper = current;
	}

	return items_[lower];
}

void Playlist::emitItemsChanged()
{
	updatePlaylistData();

	emit sigItemsChanged();
	emit sigSlidesChanged();
}

void Playlist::emitSlidesChanged()
{
	updatePlaylistData();

	emit sigSlidesChanged();
}

void Playlist::updatePlaylistData()
{
	slideCount_ = 0;
	itemOffsets_.resize(items_.count());

	for(int i = 0; i < items_.size(); i ++ ) {
		auto item = items_[i];
		item->globalSlideIdOffset_ = slideCount_;
		item->positionInPlaylist_ = i;
		itemOffsets_[i] = slideCount_;
		slideCount_ += item->slideCount();
	}
}
