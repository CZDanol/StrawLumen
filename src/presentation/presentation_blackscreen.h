#ifndef PRESENTATION_BLACKSCREEN_H
#define PRESENTATION_BLACKSCREEN_H

#include <QSharedPointer>

#include "presentation/presentation_nativepresentation.h"

class Presentation_BlackScreen : public Presentation_NativePresentation
{
	Q_OBJECT

public:
	/// Attempts to create a powerpoint presentation. Shows splashscreen during the loading. Can fail and return null.
	static QSharedPointer<Presentation_BlackScreen> create();

	~Presentation_BlackScreen();

public:
	QString identification() const override;
	QPixmap icon() const override;

	int rawSlideCount() const override;
	QPixmap rawSlideIdentificationIcon(int i) const;

private:
	Presentation_BlackScreen();

};

#endif // PRESENTATION_BLACKSCREEN_H
