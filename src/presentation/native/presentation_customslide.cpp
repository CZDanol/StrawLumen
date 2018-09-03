#include "presentation_customslide.h"

#include "presentation/native/nativeprojectorwindow.h"
#include "presentation/native/presentationpropertieswidget_customslide.h"
#include "job/settings.h"
#include "job/backgroundmanager.h"

QSharedPointer<Presentation_CustomSlide> Presentation_CustomSlide::create()
{
	QSharedPointer<Presentation_CustomSlide> result(new Presentation_CustomSlide);

	result->style_ = settings->setting_song_defaultStyle();

	result->weakPtr_ = result;
	return result;
}

QSharedPointer<Presentation_CustomSlide> Presentation_CustomSlide::createFromJSON(const QJsonObject &json)
{
	QSharedPointer<Presentation_CustomSlide> result(new Presentation_CustomSlide);
	QSignalBlocker sb(result.data());

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

	result->text_ = json["text"].toString();
	result->title_ = json["title"].toString();
	result->updateDescription();

	result->weakPtr_ = result;
	return result;
}

QJsonObject Presentation_CustomSlide::toJSON() const
{
	return QJsonObject{
		{"styleId", style_.styleId()},
		{"background", style_.hasCustomBackground() ? QJsonValue(style_.background().toJSON()) : QJsonValue()},
		{"title", title_},
		{"text", text_}
	};
}

void Presentation_CustomSlide::drawSlide(QPainter &p, int slideId, const QRect &rect)
{
	Q_UNUSED(slideId);

	style_.drawSlide(p, rect, text_, title_);
}

QString Presentation_CustomSlide::identification() const
{
	return title_.isEmpty() ? tr("Vlastní snímek") : title_;
}

QPixmap Presentation_CustomSlide::icon() const
{
	static QPixmap icon(":/icons/16/Text Box_16px.png");
	return icon;
}

QWidget *Presentation_CustomSlide::createPropertiesWidget(QWidget *parent)
{
	return new PresentationPropertiesWidget_CustomSlide(weakPtr_.toStrongRef().staticCast<Presentation_CustomSlide>(), parent);
}

int Presentation_CustomSlide::slideCount() const
{
	return 1;
}

QString Presentation_CustomSlide::slideDescription(int) const
{
	return description_;
}

QPixmap Presentation_CustomSlide::slideIdentificationIcon(int) const
{
	static QPixmap icon(":/icons/16/Text Box_16px.png");
	return icon;
}

QString Presentation_CustomSlide::classIdentifier() const
{
	return "native.customSlide";
}

Presentation_CustomSlide::Presentation_CustomSlide()
{
	connect(&style_, SIGNAL(sigChanged()), this, SLOT(onStyleChanged()));
	connect(&style_.background(), SIGNAL(sigChanged()), this, SLOT(onStyleBackgroundChanged()));
	connect(&style_, SIGNAL(sigChanged()), this, SIGNAL(sigChanged()));
	connect(&style_, &PresentationStyle::sigNeedsRepaint, this, &Presentation_CustomSlide::onStyleNeedsRepaint);
}

void Presentation_CustomSlide::updateDescription()
{
	static const QRegularExpression descRegex("\\s+");

	description_ = text_;
	description_.replace(descRegex, " ");
}

void Presentation_CustomSlide::onStyleChanged()
{
	if(signalsBlocked())
		return;

	if(isActive())
		nativeProjectorWindow->update();
}

void Presentation_CustomSlide::onStyleBackgroundChanged()
{
	backgroundManager->preloadBackground(style_.background().filename(), settings->projectionDisplayGeometry().size());
}

void Presentation_CustomSlide::onStyleNeedsRepaint()
{
	if(signalsBlocked())
		return;

	if(isActive())
		nativeProjectorWindow->update();
}
