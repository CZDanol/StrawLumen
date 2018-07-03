#ifndef PRESENTATIONENGINE_POWERPOINT_H
#define PRESENTATIONENGINE_POWERPOINT_H

#include <QAxObject>
#include <QTimer>

#include "presentationengine.h"

class Presentation_PowerPoint;

class PresentationEngine_PowerPoint : public PresentationEngine
{
	Q_OBJECT

	friend class Presentation_PowerPoint;

public:
	explicit PresentationEngine_PowerPoint(QObject *parent = nullptr);
	~PresentationEngine_PowerPoint();

public:
	bool activateEngine() override;
	void deactivateEngine() override;

private slots:
	void onActivateTimer();

private:
	QAxObject *axApplication_, *axPresentations_, *axPresentation_ = nullptr, *axPresentationWindow_, *axSlides_, *axSSSettings_;
	bool isInitialized_ = false, isValid_ = false;
	QTimer activateTimer_;

};

extern PresentationEngine_PowerPoint *presentationEngine_PowerPoint;

#endif // PRESENTATIONENGINE_POWERPOINT_H
