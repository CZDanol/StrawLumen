#ifndef PRESENTATIONENGINE_POWERPOINT_H
#define PRESENTATIONENGINE_POWERPOINT_H

#include <QAxObject>
#include <QPointF>
#include <QTimer>

#include "presentation/presentationengine.h"

class Presentation_PowerPoint;

class PresentationEngine_PowerPoint : public PresentationEngine {
	Q_OBJECT

	friend class Presentation_PowerPoint;

public:
	explicit PresentationEngine_PowerPoint(QObject *parent = nullptr);
	~PresentationEngine_PowerPoint();

public:
	virtual void activateEngine() override;
	virtual void deactivateEngine() override;

	virtual void setBlackScreen(bool set) override;
	virtual void setDisplay(QScreen *screen) override;
	virtual void raiseWindow() override;

private:
	void setDisplay_axThread(const QRectF &rect);

private slots:
	void onActivateTimer();

private:
	QAxObject *axPresentations_, *axPresentation_ = nullptr, *axPresentationWindow_, *axSlides_, *axSSSettings_, *axSSView_;
	bool isInitialized_ = false;
	QTimer activateTimer_;
};

extern PresentationEngine_PowerPoint *presentationEngine_powerPoint;

#endif// PRESENTATIONENGINE_POWERPOINT_H
