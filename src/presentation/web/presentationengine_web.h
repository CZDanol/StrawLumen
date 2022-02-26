#ifndef PRESENTATIONENGINE_WEB_H
#define PRESENTATIONENGINE_WEB_H

#include "presentation/presentationengine.h"

class PresentationEngine_Web : public PresentationEngine
{
	Q_OBJECT

public:
	explicit PresentationEngine_Web(QObject *parent =  nullptr);
	~PresentationEngine_Web() override;

public:
	virtual void activateEngine() override;
	virtual void deactivateEngine() override;

	virtual void setBlackScreen(bool set) override;
	virtual void setDisplay(const QRect &rect) override;
	virtual void raiseWindow() override;

};

extern PresentationEngine_Web *presentationEngine_web;

#endif // PRESENTATIONENGINE_WEB_H
