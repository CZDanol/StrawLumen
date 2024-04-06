#ifndef PRESENTATION_IMAGES_H
#define PRESENTATION_IMAGES_H

#include <QStandardItem>
#include <QStandardItemModel>
#include <QTimer>

#include "presentation_nativepresentation.h"

class Presentation_Images : public Presentation_NativePresentation {
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
	QPixmap specialIcon() const override;

	QWidget *createPropertiesWidget(QWidget *parent) override;

	int slideCount() const override;
	QString slideIdentification(int i) const override;
	QPixmap slideIdentificationIcon(int i) const override;
	QString slideDescription(int i) const override;

public:
	QString classIdentifier() const override;

private:
	Presentation_Images();

protected:
	virtual void activatePresentation(int startingSlide) override;
	virtual void deactivatePresentation() override;

private:
	QImage getImage(int slideId, const QSize size);
	void addImages(const QStringList &images);
	void updateTiming();

private slots:
	void onAutoTimerTimeout();

private:
	QString name_;
	QStandardItemModel images_;
	bool isAutoPresentation_ = true;
	int autoInterval_ = 2;

private:
	QTimer autoTimer_;
	int autoSlide_ = 0;
};

#endif// PRESENTATION_IMAGES_H
