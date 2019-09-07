#include "presentationbackground.h"

#include <QJsonObject>

#include "job/jsonautomation.h"
#include "job/backgroundmanager.h"

PresentationBackground::PresentationBackground()
{
	connect(backgroundManager, SIGNAL(sigBackgroundLoaded(QString)), this, SLOT(onBackgroundManagerBackgroundLoaded(QString)));
	connect(this, &PresentationBackground::sigChanged, this, &PresentationBackground::sigNeedsRepaint);
}

PresentationBackground::PresentationBackground(const PresentationBackground &other) : PresentationBackground()
{
	operator=(other);
}

QString PresentationBackground::caption() const
{
	if((color_.alpha() == 255 && blendMode_ == QPainter::CompositionMode_SourceOver) || filename_.isEmpty())
		return tr("Barva");

	if(color_.alpha() == 0)
		return tr("Obrázek");

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

int PresentationBackground::blendMode() const
{
	return blendMode_;
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

void PresentationBackground::setBlendMode(int blendMode)
{
	if(blendMode_ == blendMode)
		return;

	blendMode_ = blendMode;
	emit sigChanged();
}

void PresentationBackground::draw(QPainter &p, const QRect &rect) const
{
	p.fillRect(rect, Qt::black);

	auto bg = backgroundManager->getBackground(filename_, rect.size());
	QRect srcRect;
	srcRect.setSize(rect.size().scaled(bg.size(), Qt::KeepAspectRatio));
	srcRect.moveCenter(bg.rect().center());
	p.drawImage(rect, bg, srcRect);

	if(color_.alpha() > 0) {
		p.save();
		p.setCompositionMode((QPainter::CompositionMode) blendMode_);
		p.fillRect(rect, color_);
		p.restore();
	}
}

QJsonValue PresentationBackground::toJSON() const
{
	QJsonObject result;
	result["filename"] = filename_;
	result["color"] = ::toJSON(color_);
	result["blendMode"] = blendMode_;
	return result;
}

void PresentationBackground::loadFromJSON(const QJsonValue &json)
{
	QJsonObject obj = json.toObject();
	filename_ = obj["filename"].toString();
	::loadFromJSON(color_, obj["color"]);
	blendMode_ = obj["blendMode"].toInt(QPainter::CompositionMode_SourceOver);

	emit sigChanged();
}

bool PresentationBackground::operator==(const PresentationBackground &other) const
{
	return filename_ == other.filename_ && color_ == other.color_ && blendMode_ == other.blendMode_;
}

void PresentationBackground::operator=(const PresentationBackground &other)
{
	if(*this == other)
		return;

	filename_ = other.filename_;
	color_ = other.color_;
	blendMode_ = other.blendMode_;

	emit sigChanged();
}

void PresentationBackground::onBackgroundManagerBackgroundLoaded(const QString &filename)
{
	if(filename == filename_)
		emit sigNeedsRepaint();
}
