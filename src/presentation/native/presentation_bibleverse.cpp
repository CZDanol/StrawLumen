#include "presentation_bibleverse.h"

#include "job/settings.h"
#include "job/backgroundmanager.h"
#include "presentation/native/nativeprojectorwindow.h"
#include "presentationpropertieswidget_bibleverse.h"

QSharedPointer<Presentation_BibleVerse> Presentation_BibleVerse::create()
{
	QSharedPointer<Presentation_BibleVerse> result(new Presentation_BibleVerse);

	result->style_ = settings->setting_song_defaultStyle();

	result->weakPtr_ = result;
	return result;
}

QSharedPointer<Presentation_BibleVerse> Presentation_BibleVerse::createFromJSON(const QJsonObject &json)
{
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

	result->weakPtr_ = result;
	return result;
}

QJsonObject Presentation_BibleVerse::toJSON() const
{
	return QJsonObject{
		{"styleId", style_.styleId()},
		{"background", style_.hasCustomBackground() ? QJsonValue(style_.background().toJSON()) : QJsonValue()}
		};
}

void Presentation_BibleVerse::drawSlide(QPainter &p, int slideId, const QRect &rect)
{

}

QString Presentation_BibleVerse::identification() const
{
	return tr("Verš");
}

QPixmap Presentation_BibleVerse::icon() const
{
	static QPixmap icon(":/icons/16/Holy Bible_16px.png");
	return icon;
}

QWidget *Presentation_BibleVerse::createPropertiesWidget(QWidget *parent)
{
	return new PresentationPropertiesWidget_BibleVerse(weakPtr_.toStrongRef().staticCast<Presentation_BibleVerse>(), parent);
}

int Presentation_BibleVerse::slideCount() const
{
	return 1;
}

QString Presentation_BibleVerse::slideDescription(int i) const
{
	return nullptr;
}

QString Presentation_BibleVerse::classIdentifier() const
{
	return "native.bibleVerse";
}

Presentation_BibleVerse::Presentation_BibleVerse()
{
	connect(&style_, SIGNAL(sigChanged()), this, SLOT(onStyleChanged()));
	connect(&style_, SIGNAL(sigChanged()), this, SIGNAL(sigChanged()));
	connect(&style_.background(), SIGNAL(sigChanged()), this, SLOT(onStyleBackgroundChanged()));
	connect(&style_, &PresentationStyle::sigNeedsRepaint, this, &Presentation_BibleVerse::onStyleNeedsRepaint);
}

void Presentation_BibleVerse::onStyleChanged()
{
	if(signalsBlocked())
		return;

	if(isActive())
		nativeProjectorWindow->update();
}

void Presentation_BibleVerse::onStyleBackgroundChanged()
{
	backgroundManager->preloadBackground(style_.background().filename(), settings->projectionDisplayGeometry().size());
}

void Presentation_BibleVerse::onStyleNeedsRepaint()
{
	if(signalsBlocked())
		return;

	if(isActive())
		nativeProjectorWindow->update();
}
