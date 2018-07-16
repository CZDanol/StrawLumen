#include "presentation_song.h"

#include "job/db.h"
#include "job/backgroundmanager.h"
#include "job/settings.h"
#include "rec/chord.h"
#include "gui/projectorwindow.h"
#include "presentation/native/presentationpropertieswidget_song.h"

QSharedPointer<Presentation_Song> Presentation_Song::createFromDb(qlonglong songId)
{
	QSharedPointer<Presentation_Song> result(new Presentation_Song());
	QSignalBlocker sb(result.data());

	result->weakPtr_ = result.staticCast<Presentation_NativePresentation>();

	result->style_ = settings->setting_song_defaultStyle();
	result->emptySlideBefore_ = settings->setting_song_emptySlideBefore();
	result->emptySlideAfter_ = settings->setting_song_emptySlideAfter();

	if(!result->loadFromDb(songId))
		return nullptr;

	return result;
}

QSharedPointer<Presentation_Song> Presentation_Song::createFromJSON(const QJsonObject &json)
{
	QSharedPointer<Presentation_Song> result(new Presentation_Song());
	QSignalBlocker sb(result.data());

	result->weakPtr_ = result.staticCast<Presentation_NativePresentation>();

	if(!result->loadFromDb_Uid(json["songUid"].toString()))
		return nullptr;

	result->customSlideOrder_ = json["customSlideOrder"].toString();
	result->emptySlideBefore_ = json["emptySlideBefore"].toBool();
	result->emptySlideAfter_ = json["emptySlideAfter"].toBool();

	{
		PresentationStyle style;
		style.loadFromDb((qlonglong) json["styleId"].toDouble());
		if(!json["background"].isNull()) {
			PresentationBackground background;
			background.loadFromJSON(json["background"]);
			style.setBackground(background);
		}

		result->style_ = style;
	}

	return result;
}

QJsonObject Presentation_Song::toJSON() const
{
	return QJsonObject{
		{"songUid", songUid_},
		{"customSlideOrder", customSlideOrder_},
		{"emptySlideBefore", emptySlideBefore_},
		{"emptySlideAfter", emptySlideAfter_},
		{"styleId", style_.styleId()},
		{"background", style_.hasCustomBackground() ? QJsonValue(style_.background().toJSON()) : QJsonValue()}
	};
}

void Presentation_Song::drawSlide(QPainter &p, int slideId, const QRect &rect)
{
	style_.drawSlide(p, rect, slides_[slideId].content_, name_);
}

QString Presentation_Song::identification() const
{
	return name_;
}

QPixmap Presentation_Song::icon() const
{
	static QPixmap icon(":/icons/16/Musical Notes_16px.png");
	return icon;
}

QWidget *Presentation_Song::createPropertiesWidget(QWidget *parent)
{
	return new PresentationPropertiesWidget_Song(weakPtr_.toStrongRef().staticCast<Presentation_Song>(), parent);
}

int Presentation_Song::slideCount() const
{
	return slides_.count();
}

QString Presentation_Song::slideIdentification(int i) const
{
	return slides_[i].name_;
}

QString Presentation_Song::slideDescription(int i) const
{
	return slides_[i].description_;
}

QPixmap Presentation_Song::slideIdentificationIcon(int i) const
{
	return slides_[i].icon_;
}

QString Presentation_Song::classIdentifier() const
{
	return "native.song";
}

Presentation_Song::Presentation_Song()
{
	connect(db, &DatabaseManager::sigSongChanged, this, &Presentation_Song::onDbManagerSongChanged);
	connect(&style_, SIGNAL(sigChanged()), this, SLOT(onStyleChanged()));
	connect(&style_.background(), SIGNAL(sigChanged()), this, SLOT(onStyleBackgroundChanged()));
}

bool Presentation_Song::loadFromDb(qlonglong songId)
{
	const QSqlRecord rec = db->selectRowDef("SELECT * FROM songs WHERE id = ?", {songId});
	if(rec.isEmpty())
		return false;

	loadFromDb(rec);
	return true;
}

bool Presentation_Song::loadFromDb_Uid(const QString &uid)
{
	const QSqlRecord rec = db->selectRowDef("SELECT * FROM songs WHERE uid = ?", {uid});
	if(rec.isEmpty())
		return false;

	loadFromDb(rec);
	return true;
}

void Presentation_Song::loadFromDb(const QSqlRecord &rec)
{
	QSignalBlocker sb(this);

	songUid_ = rec.value("uid").toString();
	songId_ = rec.value("id").toLongLong();
	name_ = rec.value("name").toString();
	author_ = rec.value("author").toString();
	defaultSlideOrder_ = rec.value("slideOrder").toString();

	const bool defaultSlideOrderEmpty = defaultSlideOrder_.isEmpty();

	QString content = rec.value("content").toString();
	content.replace(songChordAnnotationRegex(),QString()); // Remove chords

	sections_.clear();
	for(const SongSectionWithContent &swc : songSectionsWithContent(content)) {
		SectionRec rec;
		rec.section = swc.section;

		for(QString content : swc.content.split(songSlideSeparatorRegex())) {
			content = content.trimmed();

			if(content.isEmpty())
				continue;

			static const QRegularExpression multiSpacesRegex("[ \t]+");
			content.replace(multiSpacesRegex, " ");

			QString desc = content;
			static const QRegularExpression descRegex("\\s+");
			desc.replace(descRegex, " ");

			rec.slides.append(SlideData{content, desc});
		}

		if(defaultSlideOrderEmpty) {
			defaultSlideOrder_.append(' ');
			defaultSlideOrder_.append(swc.section.standardName());
		}

		sections_.insert(swc.section.standardName(), rec);
	}

	loadSlideOrder();

	sb.unblock();
	emit sigItemChanged(this);
	emit sigSlidesChanged();
}

void Presentation_Song::loadSlideOrder()
{
	static const QPixmap emptySlideBeforePixmap(":/icons/16/Sign Out_16px.png");
	static const QPixmap emptySlideAfterPixmap(":/icons/16/Logout Rounded Left_16px.png");

	const QString slideOrderStr = customSlideOrder_.isEmpty() ? defaultSlideOrder_ : customSlideOrder_;
	const QStringList slideOrder = slideOrderStr.split(' ');

	slides_.clear();
	if(emptySlideBefore_)
		slides_.append(SlideRec{QString(), QString(), QString(), emptySlideBeforePixmap});

	for(const QString &sectionName : slideOrder) {
		if(sectionName.isEmpty())
			continue;

		const SectionRec &sr = sections_[sectionName];
		for(int i = 0; i < sr.slides.size(); i ++) {
			const SlideData &slideData = sr.slides[i];
			const QString slideName = sr.slides.size() > 1 ? tr("%1-%2", "%1 - section name, %2 - slide in section index (for multiple slides per section)").arg(sr.section.standardName()).arg(i+1) : sr.section.standardName();

			slides_.append(SlideRec{slideName, slideData.content_, slideData.description_});
		}
	}

	if(emptySlideAfter_)
		slides_.append(SlideRec{QString(), QString(), QString(), emptySlideAfterPixmap});

	emit sigSlidesChanged();
}

void Presentation_Song::onDbManagerSongChanged(qlonglong songId)
{
	if(songId != songId_)
		return;

	loadFromDb(songId);

	if(isActive())
		projectorWindow->update();
}

void Presentation_Song::onStyleChanged()
{
	if(signalsBlocked())
		return;

	if(isActive())
		projectorWindow->update();
}

void Presentation_Song::onStyleBackgroundChanged()
{
	backgroundManager->preloadBackground(style_.background().filename(), settings->projectionDisplayGeometry().size());
}
