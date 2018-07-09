#include "presentationbackground.h"

#include <QJsonObject>

#include "job/jsonautomation.h"
#include "job/backgroundmanager.h"

PresentationBackground::PresentationBackground()
{
	connect(backgroundManager, SIGNAL(sigBackgroundLoaded(QString)), this, SLOT(onBackgroundManagerBackgroundLoaded(QString)));
}

PresentationBackground::PresentationBackground(const PresentationBackground &other) : PresentationBackground()
{
	operator=(other);
}

QString PresentationBackground::caption() const
{
	if(color_.alpha() == 0)
		return tr("Obrázek");

	if(color_.alpha() == 255)
		return tr("Barva");

	return tr("Obrázek s %1% barvou").arg(floor(color_.alphaF() * 100));
}

const QString &PresentationBackground::filename() const
{
	return filename_;
}

const QColor &PresentationBackground::color() const
{
	return color_;
}

void PresentationBackground::setFilename(const QString &filename)
{
	if(filename_ == filename)
		return;

	filename_ = filename;
	emit sigChanged();
}

void PresentationBackground::setColor(const QColor &color)
{
	if(color_ == color)
		return;

	color_ = color;
	emit sigChanged();
}

void PresentationBackground::draw(QPainter &p, const QRect &rect) const
{
	p.fillRect(rect, Qt::black);

	if(color_.alpha() < 255)
		p.drawImage(rect, backgroundManager->getBackground(filename_, rect.size()));

	if(color_.alpha() > 0)
		p.fillRect(rect, color_);
}

QJsonValue PresentationBackground::toJSON() const
{
	QJsonObject result;
	result["filename"] = filename_;
	result["color"] = ::toJSON(color_);
	return result;
}

void PresentationBackground::loadFromJSON(const QJsonValue &json)
{
	QJsonObject obj = json.toObject();
	filename_ = obj["filename"].toString();
	::loadFromJSON(color_, obj["color"]);

	emit sigChanged();
}

bool PresentationBackground::operator==(const PresentationBackground &other) const
{
	return filename_ == other.filename_ && color_ == other.color_;
}

void PresentationBackground::operator=(const PresentationBackground &other)
{
	if(*this == other)
		return;

	filename_ = other.filename_;
	color_ = other.color_;

	emit sigChanged();
}

void PresentationBackground::onBackgroundManagerBackgroundLoaded(const QString &filename)
{
	if(filename == filename_ && color_.alpha() < 255)
		emit sigChanged();
}
