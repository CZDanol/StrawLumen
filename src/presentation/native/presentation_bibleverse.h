#ifndef PRESENTATION_BIBLEVERSE_H
#define PRESENTATION_BIBLEVERSE_H

#include "presentation/native/presentation_nativepresentation.h"

#include "rec/presentationstyle.h"

class Presentation_BibleVerse : public Presentation_NativePresentation
{
	friend class PresentationPropertiesWidget_BibleVerse;

public:
	static QSharedPointer<Presentation_BibleVerse> create();
	static QSharedPointer<Presentation_BibleVerse> createFromJSON(const QJsonObject &json);

	QJsonObject toJSON() const override;

public:
	void drawSlide(QPainter &p, int slideId, const QRect &rect) override;

public:
	QString identification() const override;
	QPixmap icon() const override;

	QWidget *createPropertiesWidget(QWidget *parent) override;

	int slideCount() const override;
	QString slideDescription(int i) const override;

public:
	QString classIdentifier() const override;

private:
	Presentation_BibleVerse();

private slots:
	void onStyleChanged();
	void onStyleBackgroundChanged();
	void onStyleNeedsRepaint();

private:
	PresentationStyle style_;

};

#endif // PRESENTATION_BIBLEVERSE_H