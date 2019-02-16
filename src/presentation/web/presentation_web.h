#ifndef PRESENTATION_WEB_H
#define PRESENTATION_WEB_H

#include <QUrl>

#include "presentation/presentation.h"

class Presentation_Web : public Presentation
{
	Q_OBJECT

	friend class PresentationPropertiesWidget_Web;

public:
	static QStringList validExtensions();

	static QSharedPointer<Presentation_Web> create();
	static QSharedPointer<Presentation_Web> createFromUrl(const QUrl &filename);
	static QSharedPointer<Presentation_Web> createFromJSON(const QJsonObject &json);

	QJsonObject toJSON() const override;

	virtual ~Presentation_Web() override;

public:
	QString identification() const override;
	QPixmap icon() const override;

	QWidget *createPropertiesWidget(QWidget *parent) override;

	int slideCount() const override;
	QPixmap slideIdentificationIcon(int i) const override;

	PresentationEngine *engine() const override;
	QString classIdentifier() const override;

	void activatePresentation(int startingSlide) override;

protected:
	Presentation_Web();

protected:
	QWeakPointer<Presentation_Web> weakPtr_;

private:
	void updateIdentification();

private:
	QString identification_;
	QUrl url_;

};

#endif // PRESENTATION_WEB_H
