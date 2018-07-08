#include "presentationbackground.h"

#include <QJsonObject>

#include "job/jsonautomation.h"
#include "job/backgroundmanager.h"

PresentationBackground::PresentationBackground()
{

}

QString PresentationBackground::caption() const
{
	if(color.alpha() == 255)
		return tr("Obrázek");

	if(color.alpha() == 0)
		return tr("Barva");

	return tr("Obrázek s %1% barvou").arg(floor(color.alphaF() * 100));
}

void PresentationBackground::draw(QPainter &p, const QRect &rect) const
{
	p.fillRect(rect, Qt::black);

	if(color.alpha() < 255)
		p.drawImage(rect, backgroundManager->background(filename, rect.size()));

	if(color.alpha() > 0)
		p.fillRect(rect, color);
}

QJsonValue PresentationBackground::toJSON() const
{
	QJsonObject result;
	result["filename"] = filename;
	result["color"] = ::toJSON(color);
	return result;
}

void PresentationBackground::loadFromJSON(const QJsonValue &json)
{
	QJsonObject obj = json.toObject();
	filename = obj["filename"].toString();
	::loadFromJSON(color, obj["color"]);
}

bool PresentationBackground::operator==(const PresentationBackground &other) const
{
	return filename == other.filename && color == other.color;
}
