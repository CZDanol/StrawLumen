#ifndef PRESENTATION_VIDEO_H
#define PRESENTATION_VIDEO_H

#include "presentation/presentation.h"

class Presentation_Video : public Presentation {
	Q_OBJECT

	friend class PresentationPropertiesWidget_Video;

public:
	static QStringList validExtensions();

	static QSharedPointer<Presentation_Video> createFromFilename(const QString &filename);
	static QSharedPointer<Presentation_Video> createFromJSON(const QJsonObject &json);

	QJsonObject toJSON() const override;

	virtual ~Presentation_Video() override;

public:
	QString identification() const override;
	QPixmap icon() const override;
	QPixmap specialIcon() const override;

	QWidget *createPropertiesWidget(QWidget *parent) override;

	int slideCount() const override;
	QPixmap slideIdentificationIcon(int i) const override;

	PresentationEngine *engine() const override;
	QString classIdentifier() const override;

	void activatePresentation(int startingSlide) override;
	void setSlide(int localSlideId, bool force = false) override;

protected:
	Presentation_Video();

protected:
	QWeakPointer<Presentation_Video> weakPtr_;

private:
	QString filename_, identification_;
	bool autoPlay_ = true, repeat_ = false;
};

#endif// PRESENTATION_VIDEO_H
