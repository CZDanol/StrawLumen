#include "presentation_web.h"

#include <QFileInfo>

#include "presentationpropertieswidget_web.h"
#include "presentationengine_web.h"
#include "webprojectorwindow.h"

QStringList Presentation_Web::validExtensions()
{
	static const QStringList result{
		"htm", "html"
	};
	return result;
}

QSharedPointer<Presentation_Web> Presentation_Web::create()
{
	QSharedPointer<Presentation_Web> result(new Presentation_Web);
	result->weakPtr_ = result;
	return result;
}

QSharedPointer<Presentation_Web> Presentation_Web::createFromUrl(const QUrl &url)
{
	QSharedPointer<Presentation_Web> result = create();
	result->url_ = url;
	result->updateIdentification();
	return result;
}

QSharedPointer<Presentation_Web> Presentation_Web::createFromJSON(const QJsonObject &json)
{
	QSharedPointer<Presentation_Web> result = create();
	result->url_ = QUrl::fromUserInput(json["url"].toString(result->url_.toString()));
	result->updateIdentification();
	return result;
}

QJsonObject Presentation_Web::toJSON() const
{
	return QJsonObject {
		{"url", url_.toString()},
	};
}

Presentation_Web::~Presentation_Web()
{

}

QString Presentation_Web::identification() const
{
	return identification_;
}

QPixmap Presentation_Web::icon() const
{
	static QPixmap icon(":/icons/16/Internet Explorer_16px.png");
	return icon;
}

QWidget *Presentation_Web::createPropertiesWidget(QWidget *parent)
{
	return new PresentationPropertiesWidget_Web(weakPtr_.toStrongRef(), parent);
}

int Presentation_Web::slideCount() const
{
	return 1;
}

QPixmap Presentation_Web::slideIdentificationIcon(int i) const
{
	Q_UNUSED(i);

	static QPixmap icon(":/icons/16/Internet Explorer_16px.png");
	return icon;
}

PresentationEngine *Presentation_Web::engine() const
{
	return presentationEngine_web;
}

QString Presentation_Web::classIdentifier() const
{
	return "web.web";
}

void Presentation_Web::activatePresentation(int startingSlide)
{
	Q_UNUSED(startingSlide);
	webProjectorWindow->setUrl(url_);
}

Presentation_Web::Presentation_Web()
{

}

void Presentation_Web::updateIdentification()
{
	if(url_.isLocalFile())
		identification_ = QFileInfo(url_.fileName()).completeBaseName();
	else
		identification_ = url_.host();
}
