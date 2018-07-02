#ifndef PRESENTATIONENGINE_POWERPOINT_H
#define PRESENTATIONENGINE_POWERPOINT_H

#include <QAxObject>

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

public:
	QAxObject *axApplication_, *axPresentations_, *axPresentation_, *axSlides_, *axSSSettings_;
	bool isInitialized_ = false, isValid_ = false;


};

extern PresentationEngine_PowerPoint *presentationEngine_PowerPoint;

#endif // PRESENTATIONENGINE_POWERPOINT_H
