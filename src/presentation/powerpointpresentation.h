#ifndef POWERPOINTPRESENTATION_H
#define POWERPOINTPRESENTATION_H

#include <QSharedPointer>
#include <QImage>
#include <QList>
#include <QStringList>

#include "presentation.h"

class PowerpointPresentation : public Presentation
{
	Q_OBJECT

public:
	static const QStringList allowedExtensions;

	/// Attempts to create a powerpoint presentation. Shows splashscreen during the loading. Can fail and return null.
	static QSharedPointer<PowerpointPresentation> create(const QString &filename);

public:
	QString identification() const override;
	QPixmap icon() const override;

	int rawSlideCount() const override;
	QString rawSlideDescription(int i) const override;

private:
	PowerpointPresentation();

private:
	int rawSlideCount_ = 0;
	QString filePath_;
	QString identification_;
	QList<QString> slideTexts_;
	QList<QImage> slideThumbnails_;

};

#endif // POWERPOINTPRESENTATION_H
