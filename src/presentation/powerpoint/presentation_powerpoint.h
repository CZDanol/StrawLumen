#ifndef PRESENTATION_POWERPOINT_H
#define PRESENTATION_POWERPOINT_H

#include <QSharedPointer>
#include <QPixmap>
#include <QList>
#include <QStringList>
#include <QFileInfo>

#include "presentation/presentation.h"

class Presentation_PowerPoint : public Presentation
{
	Q_OBJECT

	friend class PresentationPropertiesWidget_PowerPoint;

	struct Slide {
		/// Raw index in the Lumen presentation
		int index;

		/// Index in the PowerPoint presentation (can be different because of hidden slides)
		int ppIndex;

		QString text;

		QPixmap thumbnail;
	};

public:
	static const QStringList validExtensions;

	static bool isPowerpointFile(const QFileInfo &file);

	/// Attempts to create a powerpoint presentation. Shows splashscreen during the loading. Can fail and return null.
	static QSharedPointer<Presentation_PowerPoint> create(const QString &filename);

	~Presentation_PowerPoint();

public:
	QString identification() const override;
	QPixmap icon() const override;
	QPixmap specialIcon() const override;

	QWidget *createPropertiesWidget(QWidget *parent) override;

	int rawSlideCount() const override;
	QString rawSlideIdentification(int i) const override;
	QPixmap rawSlideIdentificationIcon(int i) const;
	QString rawSlideDescription(int i) const override;

	PresentationEngine *engine() const override;

	void activatePresentation(int startingSlide) override;
	void deactivatePresentation() override;
	void setSlide(int localSlideId) override;

private:
	Presentation_PowerPoint();

private:
	int rawSlideCount_ = 0;
	QString filePath_;
	QString identification_;
	QList<QSharedPointer<Slide>> slides_;
	QWeakPointer<Presentation_PowerPoint> weakPtr_;
	bool isAutoPresentation_ = false;

};

#endif // PRESENTATION_POWERPOINT_H
