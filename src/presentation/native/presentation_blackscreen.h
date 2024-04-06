#ifndef PRESENTATION_BLACKSCREEN_H
#define PRESENTATION_BLACKSCREEN_H

#include <QSharedPointer>

#include "presentation/native/presentation_nativepresentation.h"

class Presentation_BlackScreen : public Presentation_NativePresentation {
	Q_OBJECT

public:
	/// Attempts to create a powerpoint presentation. Shows splashscreen during the loading. Can fail and return null.
	static QSharedPointer<Presentation_BlackScreen> create();

	~Presentation_BlackScreen();

	QJsonObject toJSON() const override;

public:
	void drawSlide(QPainter &p, int slideId, const QRect &rect) override;

public:
	QString identification() const override;
	QPixmap icon() const override;

	int slideCount() const override;
	QPixmap slideIdentificationIcon(int i) const;

public:
	QString classIdentifier() const override;

private:
	Presentation_BlackScreen();
};

#endif// PRESENTATION_BLACKSCREEN_H
