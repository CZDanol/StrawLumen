#include "playlist.h"

#include <QJsonArray>

#include "presentation/presentation.h"
#include "presentation/powerpoint/presentation_powerpoint.h"
#include "presentation/native/presentation_song.h"
#include "presentation/native/presentation_blackscreen.h"
#include "presentation/native/presentation_customslide.h"
#include "presentation/native/presentation_bibleverse.h"
#include "presentation/native/presentation_images.h"
#include "presentation/video/presentation_video.h"
#include "presentation/web/presentation_web.h"
#include "util/standarddialogs.h"
#include "job/db.h"

Playlist::Playlist()
{
	clear();

	connect(this, SIGNAL(sigItemsChanged()), this, SIGNAL(sigChanged()));
	connect(this, SIGNAL(sigChanged()), this, SLOT(onChanged()));
	connect(db, &DatabaseManager::sigPlaylistRenamed, this, &Playlist::onPlaylistRenamed);
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
	connect(item.data(), SIGNAL(sigChanged()), this, SIGNAL(sigChanged()));
	connect(item.data(), &Presentation::sigMorphedInto, this, &Playlist::onPresentationMorphedInto);

	emitItemsChanged();
	emit sigItemsAdded();
	return true;
}

void Playlist::addItems(const QVector<QSharedPointer<Presentation> > &items)
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
		connect(item.data(), SIGNAL(sigChanged()), this, SIGNAL(sigChanged()));
		connect(item.data(), &Presentation::sigMorphedInto, this, &Playlist::onPresentationMorphedInto);
	}

	emitItemsChanged();
	emit sigItemsAdded();
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
	items_.removeAt(item->positionInPlaylist_);

	emitItemsChanged();
}

void Playlist::insertItems(int pos, const QVector<QSharedPointer<Presentation> > &items)
{
	if(pos > items_.count() || pos < 0)
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
	emit sigItemsAdded();
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

void Playlist::clear()
{
	for(const QSharedPointer<Presentation> &item : items_)
		item->playlist_ = nullptr;

	items_.clear();
	itemOffsets_.clear();
	slideCount_ = 0;

	dbId = -1;
	dbName = tr("Nový program");

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

QJsonObject Playlist::toJSON() const
{
	QJsonObject json;
	QJsonArray itemsJson;

	for(const QSharedPointer<Presentation> &item : items_) {
		QJsonObject itemJson;
		itemJson["identification"] = item->identification();
		itemJson["classIdentifier"] = item->classIdentifier();
		itemJson["data"] = item->toJSON();
		itemsJson.append(itemJson);
	}

	json["items"] = itemsJson;

	return json;
}

bool Playlist::loadFromJSON(const QJsonObject &json)
{
#define F(result) [](const QJsonObject &json){Q_UNUSED(json); return result;}
	static const QHash<QString,std::function<QSharedPointer<Presentation>(const QJsonObject &obj)> > classConstructors {
		{"powerPoint.powerPoint", F(Presentation_PowerPoint::createFromJSON(json))},

		{"native.song", F(Presentation_Song::createFromJSON(json))},
		{"native.bibleVerse", F(Presentation_BibleVerse::createFromJSON(json))},
		{"native.blackScreen", F(Presentation_BlackScreen::create())},
		{"native.customSlide", F(Presentation_CustomSlide::createFromJSON(json))},
		{"native.images", F(Presentation_Images::createFromJSON(json))},

		{"video.video", F(Presentation_Video::createFromJSON(json))},

		{"web.web", F(Presentation_Web::createFromJSON(json))},
	};
#undef F

	QSignalBlocker sb(this);
	clear();

	QStringList failedToLoadList;

	const QJsonArray itemsJson = json["items"].toArray();
	for(int i = 0; i < itemsJson.size(); i++) {
		const QJsonObject itemJson = itemsJson[i].toObject();
		const QString classIdentifier = itemJson["classIdentifier"].toString();

		if(!classConstructors.contains(classIdentifier)) {
			failedToLoadList << itemJson["identification"].toString();
			standardErrorDialog(tr("Neznámý typ prezentace: \"%1\"").arg(classIdentifier));
			continue;
		}

		QSharedPointer<Presentation> item = classConstructors[classIdentifier](itemJson["data"].toObject());
		if(!item) {
			failedToLoadList << itemJson["identification"].toString();
			continue;
		}

		addItem(item);
	}

	sb.unblock();

	emitItemsChanged();
	emit sigItemsAdded();

	if(!failedToLoadList.empty())
		standardErrorDialog(tr("Níže vyčtené položky se nepodařilo načíst:\n\n%1").arg(failedToLoadList.join('\n')));

	return failedToLoadList.empty();
}

QStringList Playlist::itemNamesFromJSON(const QJsonObject &json)
{
	QStringList result;
	for(const auto itemJson : json["items"].toArray())
		result << itemJson.toObject()["identification"].toString();

	return result;
}

bool Playlist::areChangesSaved() const
{
	return areChangesSaved_;
}

void Playlist::assumeChangesSaved()
{
	areChangesSaved_ = true;
	emit sigSaved();
}

void Playlist::emitItemsChanged()
{
	if(signalsBlocked())
		return;

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

void Playlist::onChanged()
{
	areChangesSaved_ = false;
}

void Playlist::onPlaylistRenamed(qlonglong id, const QString &newName)
{
	if(id != -1 && dbId == id) {
		dbName = newName;
		emit sigNameChanged();
	}
}

void Playlist::onPresentationMorphedInto(const QSharedPointer<Presentation> &from, const QSharedPointer<Presentation> &to)
{
	int ix = items_.indexOf(from);
	if(ix == -1)
		return;

	items_[ix] = to;
	emitItemsChanged();
}
