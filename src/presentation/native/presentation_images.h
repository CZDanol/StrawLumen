#ifndef PRESENTATION_IMAGES_H
#define PRESENTATION_IMAGES_H

#include <QStandardItemModel>
#include <QStandardItem>

#include "presentation_nativepresentation.h"

class Presentation_Images : public Presentation_NativePresentation
{
	Q_OBJECT

	friend class PresentationPropertiesWidget_Images;

public:
	static QStringList validExtensions();

	static QSharedPointer<Presentation_Images> create();
	static QSharedPointer<Presentation_Images> create(const QStringList &files);
	static QSharedPointer<Presentation_Images> createFromJSON(const QJsonObject &json);

	QJsonObject toJSON() const override;

public:
	void drawSlide(QPainter &p, int slideId, const QRect &rect) override;

public:
	QString identification() const override;
	QPixmap icon() const override;

	QWidget *createPropertiesWidget(QWidget *parent) override;

	int slideCount() const override;
	QString slideIdentification(int i) const override;
	QString slideDescription(int i) const override;

public:
	QString classIdentifier() const override;

private:
	Presentation_Images();

private:
	void addImages(const QStringList &images);

private:
	QStandardItemModel images_;

};

#endif // PRESENTATION_IMAGES_H
