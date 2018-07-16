#include "presentationengine_powerpoint.h"

#include "util/standarddialogs.h"
#include "job/activexjobthread.h"
#include "presentation/presentationmanager.h"
#include "presentation/powerpoint/presentation_powerpoint.h"

PresentationEngine_PowerPoint *presentationEngine_PowerPoint = nullptr;

PresentationEngine_PowerPoint::PresentationEngine_PowerPoint(QObject *parent) : PresentationEngine(parent)
{
	activateTimer_.setInterval(1000);
	connect(&activateTimer_, SIGNAL(timeout()), this, SLOT(onActivateTimer()));
}

PresentationEngine_PowerPoint::~PresentationEngine_PowerPoint()
{
	// Prevent destroying the engine before deactivateEngine is possibly called on the activeX thread
	activeXJobThread->waitJobsDone();
}

void PresentationEngine_PowerPoint::activateEngine()
{
#ifndef QT_DEBUG
	// For some reason, the application loses focus when the PPT is activated, but only in debug mode
	activateTimer_.start();
#endif

	if(isInitialized_)
		return;

	isInitialized_ = true;

	activeXJobThread->executeNonblocking([&]{
		auto axApplication = activeXJobThread->axPowerPointApplication;

		axPresentations_ = axApplication->querySubObject("Presentations");
	});
}

void PresentationEngine_PowerPoint::deactivateEngine()
{
	activateTimer_.stop();
}

void PresentationEngine_PowerPoint::setBlackScreen(bool set)
{
	activeXJobThread->executeNonblocking([=]{
		if(axPresentation_)
			axSSView_->dynamicCall("SetState(int)", (set || presentationManager->currentPresentation().dynamicCast<Presentation_PowerPoint>()->isSlideBlackScreen(presentationManager->currentLocalSlideId())) ? (int) Office::PowerPoint::PpSlideShowState::ppSlideShowBlackScreen : (int) Office::PowerPoint::PpSlideShowState::ppSlideShowRunning);
	});
}

void PresentationEngine_PowerPoint::setDisplay(const QRect &rect)
{
	activeXJobThread->executeNonblocking([this, rect]{
		setDisplay_axThread(rect);
	});
}

void PresentationEngine_PowerPoint::setDisplay_axThread(const QRect &rect)
{
	if(!axPresentation_)
		return;

	// Magical constant that makes everything work. Microsoft...
	const double ratio = (3.0/4.0);
	axPresentationWindow_->dynamicCall("SetLeft(double)", (double) rect.left() * ratio);
	axPresentationWindow_->dynamicCall("SetTop(double)", (double) rect.top() * ratio);
	axPresentationWindow_->dynamicCall("SetWidth(double)", (double) rect.width() * ratio);
	axPresentationWindow_->dynamicCall("SetHeight(double)", (double) rect.height() * ratio);
}

void PresentationEngine_PowerPoint::onActivateTimer()
{
	activeXJobThread->executeNonblocking([=]{
		if(axPresentation_)
			axPresentationWindow_->dynamicCall("Activate()");
	});
}
