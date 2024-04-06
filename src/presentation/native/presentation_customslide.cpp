#include "presentation_customslide.h"

#include "job/backgroundmanager.h"
#include "job/settings.h"
#include "presentation/native/nativeprojectorwindow.h"
#include "presentation/native/presentationpropertieswidget_customslide.h"

QSharedPointer<Presentation_CustomSlide> Presentation_CustomSlide::create() {
	QSharedPointer<Presentation_CustomSlide> result(new Presentation_CustomSlide);

	result->style_ = settings->setting_song_defaultStyle();
	result->text_ = tr("{Snímek 1}\nText\n\n{Snímek 2}\nText");
	result->updateSlides();

	result->weakPtr_ = result;
	return result;
}

QSharedPointer<Presentation_CustomSlide> Presentation_CustomSlide::createFromJSON(const QJsonObject &json) {
	QSharedPointer<Presentation_CustomSlide> result(new Presentation_CustomSlide);
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

	result->text_ = json["text"].toString();
	result->wordWrap_ = json["wordWrap"].toBool();
	result->updateSlides();

	result->weakPtr_ = result;
	return result;
}

QJsonObject Presentation_CustomSlide::toJSON() const {
	return QJsonObject{
	  {"styleId", style_.styleId()},
	  {"background", style_.hasCustomBackground() ? QJsonValue(style_.background().toJSON()) : QJsonValue()},
	  {"text", text_},
	  {"wordWrap", wordWrap_}};
}

void Presentation_CustomSlide::drawSlide(QPainter &p, int slideId, const QRect &rect) {
	Q_UNUSED(slideId);

	style_.drawSlide(p, rect, slides_[slideId], titles_[slideId], wordWrap_ ? PresentationStyle::fWordWrapContent : 0);
}

QString Presentation_CustomSlide::identification() const {
	return tr("Vlastní text");
}

QPixmap Presentation_CustomSlide::icon() const {
	static QPixmap icon(":/icons/16/Text Box_16px.png");
	return icon;
}

QWidget *Presentation_CustomSlide::createPropertiesWidget(QWidget *parent) {
	return new PresentationPropertiesWidget_CustomSlide(weakPtr_.toStrongRef().staticCast<Presentation_CustomSlide>(), parent);
}

int Presentation_CustomSlide::slideCount() const {
	return slides_.length();
}

QString Presentation_CustomSlide::slideDescription(int slideId) const {
	return descriptions_[slideId];
}

QString Presentation_CustomSlide::slideIdentification(int slideId) const {
	return titles_[slideId];
}

QString Presentation_CustomSlide::classIdentifier() const {
	return "native.customSlide";
}

void Presentation_CustomSlide::setText(const QString &set) {
	text_ = set;
	updateSlides();
	emit sigItemChanged(this);
}

Presentation_CustomSlide::Presentation_CustomSlide() {
	connect(&style_, SIGNAL(sigChanged()), this, SLOT(onStyleChanged()));
	connect(&style_, SIGNAL(sigChanged()), this, SIGNAL(sigChanged()));
	connect(&style_.background(), SIGNAL(sigChanged()), this, SLOT(onStyleBackgroundChanged()));
	connect(&style_, &PresentationStyle::sigNeedsRepaint, this, &Presentation_CustomSlide::onStyleNeedsRepaint);
}

void Presentation_CustomSlide::updateSlides() {
	static const QRegularExpression descRegex("\\s+");
	static const QRegularExpression sectionRegex("^\\{\\s*(.*?)\\s*\\}\\s*$", QRegularExpression::MultilineOption);

	slides_.clear();
	descriptions_.clear();
	titles_.clear();

	titles_ += nullptr;

	int lastPos = 0;
	auto f = [&](int pos) {
		QString text = text_.mid(lastPos, pos - lastPos).trimmed();
		slides_ += text;

		text.replace(descRegex, " ");
		descriptions_ += text;
	};

	auto i = sectionRegex.globalMatch(text_);
	while(i.hasNext()) {
		const auto m = i.next();
		f(m.capturedStart(0));
		titles_ += m.captured(1);
		lastPos = m.capturedEnd(0);
	}

	f(text_.length());

	if(slides_.first().isEmpty() && titles_.first().isEmpty()) {
		slides_.removeFirst();
		titles_.removeFirst();
		descriptions_.removeFirst();
	}
}

void Presentation_CustomSlide::onStyleChanged() {
	if(signalsBlocked())
		return;

	if(isActive())
		nativeProjectorWindow->update();
}

void Presentation_CustomSlide::onStyleBackgroundChanged() {
	backgroundManager->preloadBackground(style_.background().filename(), settings->projectionDisplayGeometry().size());
}

void Presentation_CustomSlide::onStyleNeedsRepaint() {
	if(signalsBlocked())
		return;

	if(isActive())
		nativeProjectorWindow->update();
}
