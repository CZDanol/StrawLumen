#ifndef PRESENTATION_CUSTOMSLIDE_H
#define PRESENTATION_CUSTOMSLIDE_H

#include "presentation/native/presentation_nativepresentation.h"

#include "rec/presentationstyle.h"

class Presentation_CustomSlide : public Presentation_NativePresentation
{
	Q_OBJECT

	friend class PresentationPropertiesWidget_CustomSlide;

public:
	static QSharedPointer<Presentation_CustomSlide> create();
	static QSharedPointer<Presentation_CustomSlide> createFromJSON(const QJsonObject &json);

	QJsonObject toJSON() const override;

public:
	void drawSlide(QPainter &p, int slideId, const QRect &rect) override;

public:
	QString identification() const override;
	QPixmap icon() const override;

	QWidget *createPropertiesWidget(QWidget *parent) override;

	int slideCount() const override;
	QString slideDescription(int i) const override;
	QPixmap slideIdentificationIcon(int i) const override;

public:
	QString classIdentifier() const override;

private:
	Presentation_CustomSlide();

private slots:
	void updateDescription();

private slots:
	void onStyleChanged();
	void onStyleBackgroundChanged();
	void onStyleNeedsRepaint();

private:
	PresentationStyle style_;
	QString title_, text_;
	QString description_;

};

#endif // PRESENTATION_CUSTOMSLIDE_H
