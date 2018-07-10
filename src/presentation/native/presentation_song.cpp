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
	result->style_ = settings->defaultPresentationStyle();
	result->emptySlideBefore_ = settings->value("song.emptySlideBefore").toBool();
	result->emptySlideAfter_ = settings->value("song.emptySlideAfter", true).toBool();

	result->loadFromDb(songId);

	return result;
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

Presentation_Song::Presentation_Song()
{
	connect(db, &DatabaseManager::sigSongChanged, this, &Presentation_Song::onDbManagerSongChanged);
	connect(&style_, SIGNAL(sigChanged()), this, SLOT(onStyleChanged()));
	connect(&style_.background(), SIGNAL(sigChanged()), this, SLOT(onStyleBackgroundChanged()));
}

void Presentation_Song::loadFromDb(qlonglong songId)
{
	const QSqlRecord r = db->selectRowDef("SELECT * FROM songs WHERE id = ?", {songId});
	if(r.isEmpty())
		return;

	QSignalBlocker sb(this);

	songId_ = songId;
	name_ = r.value("name").toString();
	author_ = r.value("author").toString();
	defaultSlideOrder_ = r.value("slideOrder").toString();

	const bool defaultSlideOrderEmpty = defaultSlideOrder_.isEmpty();

	QString content = r.value("content").toString();
	content.replace(songChordRegex(),QString()); // Remove chords

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
	static const QPixmap emptySlidePixmap(":/icons/16/Circled Thin_16px.png");

	const QString slideOrderStr = customSlideOrder_.isEmpty() ? defaultSlideOrder_ : customSlideOrder_;
	const QStringList slideOrder = slideOrderStr.split(' ');

	slides_.clear();
	if(emptySlideBefore_)
		slides_.append(SlideRec{QString(), QString(), QString(), emptySlidePixmap});

	for(const QString &sectionName : slideOrder) {
		if(sectionName.isEmpty())
			continue;

		const SectionRec &sr = sections_[sectionName];
		for(const SlideData &slideData : sr.slides)
			slides_.append(SlideRec{sr.section.standardName(), slideData.content_, slideData.description_});
	}

	if(emptySlideAfter_)
		slides_.append(SlideRec{QString(), QString(), QString(), emptySlidePixmap});

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
