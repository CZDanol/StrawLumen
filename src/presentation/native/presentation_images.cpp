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
	const QString fileName = images_.item(slideId)->data().toString();
	const QSize targetSize = rect.size();

	QString cacheKey = QString("image:%1#res:%2x%3").arg(fileName).arg(targetSize.width()).arg(targetSize.height());
	auto produceFunction = [fileName, targetSize](QVariant &result, int &price, AsyncCacheManager::CheckStornoFunction storno){
		QImage img(fileName);

		if(storno())
			return false;

		img = img.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		result = img;
		price = 1 + img.width() * img.height() * 4;

		return true;
	};

	QImage img = asyncCache->request(cacheKey, produceFunction, projectorWindow, []{projectorWindow->update();}).value<QImage>();

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

QWidget *Presentation_Images::createPropertiesWidget(QWidget *parent)
{
	return new PresentationPropertiesWidget_Images(weakPtr_.toStrongRef().staticCast<Presentation_Images>(), parent);
}

int Presentation_Images::slideCount() const
{
	return images_.rowCount();
}

QString Presentation_Images::slideIdentification(int i) const
{
	return QString::number(i+1);
}

QString Presentation_Images::slideDescription(int i) const
{
	return images_.item(i)->text();
}

QString Presentation_Images::classIdentifier() const
{
	return "native.images";
}

Presentation_Images::Presentation_Images()
{

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
