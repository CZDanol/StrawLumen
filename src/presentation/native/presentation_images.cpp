#include "presentation_images.h"

#include <QFileInfo>
#include <QImage>
#include <QJsonArray>

#include "presentationpropertieswidget_images.h"
#include "job/asynccachemanager.h"
#include "gui/projectorwindow.h"

QStringList Presentation_Images::validExtensions()
{
	static const QStringList result{
		"jpg", "jpeg", "png", "gif", "bmp"
	};
	return result;
}

QSharedPointer<Presentation_Images> Presentation_Images::create()
{
	QSharedPointer<Presentation_Images> result(new Presentation_Images);
	result->weakPtr_ = result;
	return result;
}

QSharedPointer<Presentation_Images> Presentation_Images::create(const QStringList &files)
{
	QSharedPointer<Presentation_Images> result = create();
	result->addImages(files);
	return result;
}

QSharedPointer<Presentation_Images> Presentation_Images::createFromJSON(const QJsonObject &json)
{
	QSharedPointer<Presentation_Images> result = create();

	QStringList images;
	for(const auto image : json["images"].toArray())
		images += image.toString();
	result->addImages(images);

	result->isAutoPresentation_ = json["autoPresentation"].toBool(result->isAutoPresentation_);
	result->autoInterval_ = json["autoInterval"].toInt(result->autoInterval_);

	return result;
}

QJsonObject Presentation_Images::toJSON() const
{
	QJsonObject json;

	{
		QJsonArray images;
		for(int i = 0; i < images_.rowCount(); i++)
			images.append(images_.item(i)->data().toString());

		json["images"] = images;
	}

	return json;
}

void Presentation_Images::drawSlide(QPainter &p, int slideId, const QRect &rect)
{
	if(images_.rowCount() == 0)
		return;

	if(isAutoPresentation_)
		slideId = autoSlide_ % images_.rowCount();

	QImage img = getImage(slideId, rect.size());

	// Preload next slide
	getImage((slideId+1) % images_.rowCount(), rect.size());

	if(img.isNull())
		return;

	QRect rct;
	rct.setSize(img.size());
	rct.moveCenter(rect.center());

	p.drawImage(rct, img);
}

QString Presentation_Images::identification() const
{
	return tr("Obrázky");
}

QPixmap Presentation_Images::icon() const
{
	static QPixmap icon(":/icons/16/Photo Gallery_16px.png");
	return icon;
}

QPixmap Presentation_Images::specialIcon() const
{
	static QPixmap autoPresPixmap(":/icons/16/Repeat_16px.png");

	if(isAutoPresentation_)
		return autoPresPixmap;

	return QPixmap();
}

QWidget *Presentation_Images::createPropertiesWidget(QWidget *parent)
{
	return new PresentationPropertiesWidget_Images(weakPtr_.toStrongRef().staticCast<Presentation_Images>(), parent);
}

int Presentation_Images::slideCount() const
{
	if(isAutoPresentation_)
		return 1;

	return images_.rowCount();
}

QString Presentation_Images::slideIdentification(int i) const
{
	if(isAutoPresentation_)
		return nullptr;

	return QString::number(i+1);
}

QPixmap Presentation_Images::slideIdentificationIcon(int i) const
{
	Q_UNUSED(i);

	static QPixmap autoPresPixmap(":/icons/16/Repeat_16px.png");

	if(isAutoPresentation_)
		return autoPresPixmap;

	return QPixmap();
}

QString Presentation_Images::slideDescription(int i) const
{
	if(isAutoPresentation_)
		return nullptr;

	return images_.item(i)->text();
}

QString Presentation_Images::classIdentifier() const
{
	return "native.images";
}

Presentation_Images::Presentation_Images()
{
	connect(&autoTimer_, &QTimer::timeout, this, &Presentation_Images::onAutoTimerTimeout);
}

void Presentation_Images::activatePresentation(int startingSlide)
{
	Presentation_NativePresentation::activatePresentation(startingSlide);
	updateTiming();
}

void Presentation_Images::deactivatePresentation()
{
	Presentation_NativePresentation::deactivatePresentation();
	updateTiming();
}

QImage Presentation_Images::getImage(int slideId, const QSize size)
{
	const QString fileName = images_.item(slideId)->data().toString();

	QString cacheKey = QString("image:%1#res:%2x%3").arg(fileName).arg(size.width()).arg(size.height());
	auto produceFunction = [fileName, size](QVariant &result, int &price, AsyncCacheManager::CheckStornoFunction storno){
		QImage img(fileName);

		if(storno())
			return false;

		img = img.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		result = img;
		price = 1 + img.width() * img.height() * 4;

		return true;
	};

	return asyncCache->request(cacheKey, produceFunction, projectorWindow, []{projectorWindow->update();}).value<QImage>();
}

void Presentation_Images::addImages(const QStringList &images)
{
	for(const QString &image : images) {
		QStandardItem *i = new QStandardItem(QFileInfo(image).baseName());
		i->setFlags((i->flags() & ~(Qt::ItemIsEditable | Qt::ItemIsDropEnabled)) | Qt::ItemNeverHasChildren);
		i->setData(image);

		images_.appendRow(i);
	}

	emit sigSlidesChanged();
}

void Presentation_Images::updateTiming()
{
	if(!isActive() || !isAutoPresentation_)
		autoTimer_.stop();

	else {
		autoTimer_.setInterval(autoInterval_ * 1000);
		autoTimer_.start();
	}
}

void Presentation_Images::onAutoTimerTimeout()
{
	autoSlide_++;
	projectorWindow->update();
}