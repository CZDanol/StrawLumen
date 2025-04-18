#include "presentation_bibleverse.h"

#include "job/backgroundmanager.h"
#include "job/settings.h"
#include "presentation/native/nativeprojectorwindow.h"
#include "presentation/native/presentation_customslide.h"
#include "presentationpropertieswidget_bibleverse.h"
#include "rec/bibleref.h"

QSharedPointer<Presentation_BibleVerse> Presentation_BibleVerse::create() {
	QSharedPointer<Presentation_BibleVerse> result(new Presentation_BibleVerse);

	result->style_ = settings->setting_song_defaultStyle();

	result->weakPtr_ = result;
	return result;
}

QSharedPointer<Presentation_BibleVerse> Presentation_BibleVerse::createFromJSON(const QJsonObject &json) {
	QSharedPointer<Presentation_BibleVerse> result(new Presentation_BibleVerse);
	QSignalBlocker sb(result.data());

	{
		PresentationStyle style;
		style.loadFromDb(static_cast<qlonglong>(json["styleId"].toDouble()));
		if(!json["background"].isNull()) {
			PresentationBackground background;
			background.loadFromJSON(json["background"]);
			style.setBackground(background);
		}

		result->style_ = style;
	}

	result->versesStr_ = json["verses"].toString();
	result->updateVerses();

	result->weakPtr_ = result;
	return result;
}

QJsonObject Presentation_BibleVerse::toJSON() const {
	return {
	  {"styleId", style_.styleId()},
	  {"background", (style_.hasCustomBackground() ? QJsonValue(style_.background().toJSON()) : QJsonValue())},
	  {"verses", versesStr_},
	};
}

void Presentation_BibleVerse::setVersesStr(const QString &set, bool defaultEmptySlide) {
	if(versesStr_ == set && defaultEmptySlide_ == defaultEmptySlide)
		return;

	versesStr_ = set;
	defaultEmptySlide_ = defaultEmptySlide;
	updateVerses(defaultEmptySlide);
}

void Presentation_BibleVerse::drawSlide(QPainter &p, int slideId, const QRect &rect) {
	style_.drawSlide(p, rect, slides_[slideId], slideNames_[slideId], PresentationStyle::fWordWrapContent);
}

QString Presentation_BibleVerse::identification() const {
	return tr("Verš");
}

QPixmap Presentation_BibleVerse::icon() const {
	static QPixmap icon(":/icons/16/Holy Bible_16px.png");
	return icon;
}

QWidget *Presentation_BibleVerse::createPropertiesWidget(QWidget *parent) {
	return new PresentationPropertiesWidget_BibleVerse(weakPtr_.toStrongRef().staticCast<Presentation_BibleVerse>(), parent);
}

int Presentation_BibleVerse::slideCount() const {
	return slides_.size();
}

QString Presentation_BibleVerse::slideIdentification(int i) const {
	return slideNames_[i];
}

QString Presentation_BibleVerse::slideDescription(int i) const {
	return slideDescriptions_[i];
}

QString Presentation_BibleVerse::classIdentifier() const {
	return "native.bibleVerse";
}

QSharedPointer<Presentation_CustomSlide> Presentation_BibleVerse::toCustomSlide() const {
	QSharedPointer<Presentation_CustomSlide> result = Presentation_CustomSlide::create();

	QString text;
	for(int i = 0; i < slideCount(); i++)
		text += QString("{%1}\n%2\n\n").arg(slideNames_[i], slides_[i]);

	result->setText(text);
	return result;
}

Presentation_BibleVerse::Presentation_BibleVerse() {
	connect(&style_, SIGNAL(sigChanged()), this, SLOT(onStyleChanged()));
	connect(&style_, SIGNAL(sigChanged()), this, SIGNAL(sigChanged()));
	connect(&style_.background(), SIGNAL(sigChanged()), this, SLOT(onStyleBackgroundChanged()));
	connect(&style_, &PresentationStyle::sigNeedsRepaint, this, &Presentation_BibleVerse::onStyleNeedsRepaint);
}

void Presentation_BibleVerse::updateVerses(bool defaultEmptySlide) {
	QString slide, slideName;

	slides_.clear();
	slideNames_.clear();
	slideDescriptions_.clear();

	for(const QString &refStr: versesStr_.split('\n')) {
		if(refStr.trimmed().isEmpty() && !slide.isEmpty()) {
			slides_ += slide;
			slideNames_ += slideName;

			slide.clear();
			slideName.clear();
		}

		BibleRef ref(refStr);
		if(!ref.isValid())
			continue;

		if(!slideName.isEmpty()) {
			slide += "\n \n";
			slideName += ", ";
		}

		slide += ref.contentString();
		slideName += ref.toString();

		slide = slide.trimmed();
	}

	if(!slide.isEmpty()) {
		slides_ += slide;
		slideNames_ += slideName;
	}

	if(defaultEmptySlide && slides_.isEmpty()) {
		slides_ += QString{};
		slideNames_ += QString{};
	}

	for(QString &slide: slides_) {
		QString slideDesc = slide.left(400).trimmed();
		slideDesc.remove('\n');
		slideDescriptions_ += slideDesc;

		/*
		static const QRegularExpression wrapRegex(
					"("
					".{100}.*?" // Wrap on the end of the next word after 100 characters
					"(?:\\b[a-z] \\w+\\s+|\\s+)" // If the last word is one symbol, take the next word too
					")", QRegularExpression::UseUnicodePropertiesOption | QRegularExpression::CaseInsensitiveOption);
		slide.replace(wrapRegex, "\\1\n");*/
	}
}

void Presentation_BibleVerse::onStyleChanged() {
	if(signalsBlocked())
		return;

	if(isActive())
		nativeProjectorWindow->update();
}

void Presentation_BibleVerse::onStyleBackgroundChanged() {
	backgroundManager->preloadBackground(style_.background().filename(), settings->projectionDisplayGeometry().size());
}

void Presentation_BibleVerse::onStyleNeedsRepaint() {
	if(signalsBlocked())
		return;

	if(isActive())
		nativeProjectorWindow->update();
}
