#ifndef PRESENTATION_CUSTOMSLIDE_H
#define PRESENTATION_CUSTOMSLIDE_H

#include "presentation/native/presentation_nativepresentation.h"

#include "rec/presentationstyle.h"

class Presentation_CustomSlide : public Presentation_NativePresentation {
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
	QString slideDescription(int slideId) const override;
	QString slideIdentification(int slideId) const override;

public:
	QString classIdentifier() const override;

public:
	void setText(const QString &set);

private:
	Presentation_CustomSlide();

private slots:
	void updateSlides();

private slots:
	void onStyleChanged();
	void onStyleBackgroundChanged();
	void onStyleNeedsRepaint();

private:
	PresentationStyle style_;
	QString text_;
	QStringList slides_, titles_, descriptions_;
	bool wordWrap_ = true;
};

#endif// PRESENTATION_CUSTOMSLIDE_H
