#ifndef PRESENTATION_POWERPOINT_H
#define PRESENTATION_POWERPOINT_H

#include <QFileInfo>
#include <QList>
#include <QPixmap>
#include <QSharedPointer>
#include <QStringList>

#include "presentation/presentation.h"

class Presentation_PowerPoint : public Presentation {
	Q_OBJECT

	friend class PresentationPropertiesWidget_PowerPoint;

public:
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
	static QSharedPointer<Presentation_PowerPoint> createFromFilename(const QString &filename, bool useSplashscreen = true, bool useSplashscreenStorno = true);
	static QSharedPointer<Presentation_PowerPoint> createFromJSON(const QJsonObject &json);

	QJsonObject toJSON() const override;

	~Presentation_PowerPoint();

public:
	QString identification() const override;
	QPixmap icon() const override;
	QPixmap specialIcon() const override;

	QWidget *createPropertiesWidget(QWidget *parent) override;

	inline const auto &getSlides() const {
		return slides_;
	}

	int slideCount() const override;
	QString slideIdentification(int i) const override;
	QPixmap slideIdentificationIcon(int i) const override;
	QString slideDescription(int i) const override;

	PresentationEngine *engine() const override;
	QString classIdentifier() const override;

	void activatePresentation(int startingSlide) override;
	void deactivatePresentation() override;
	void setSlide(int localSlideId, bool force = false) override;

public:
	bool isSlideBlackScreen(int localSlideId) const;

private:
	Presentation_PowerPoint();

private:
	/// -1 -> blackScreen, -2 -> do nothing (auto presentation)
	void setSlide_axThread(int getPptSlideI);
	int getPptSlideI(int localSlideId) const;

private:
	QString filePath_;
	bool isAutoPresentation_ = false;
	bool blackSlideBefore_, blackSlideAfter_;

private:
	int slideCount_ = 0;
	QString identification_;
	QList<QSharedPointer<Slide>> slides_;
	QWeakPointer<Presentation_PowerPoint> weakPtr_;
};

#endif// PRESENTATION_POWERPOINT_H
