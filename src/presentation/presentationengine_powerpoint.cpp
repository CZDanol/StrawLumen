#include "presentationengine_powerpoint.h"

#include "util/standarddialogs.h"
#include "job/activexjobthread.h"

PresentationEngine_PowerPoint *presentationEngine_PowerPoint = nullptr;

PresentationEngine_PowerPoint::PresentationEngine_PowerPoint(QObject *parent) : PresentationEngine(parent)
{
	activateTimer_.setInterval(100);
	connect(&activateTimer_, SIGNAL(timeout()), this, SLOT(onActivateTimer()));
}

PresentationEngine_PowerPoint::~PresentationEngine_PowerPoint()
{
	// Prevent destroying the engine before deactivateEngine is possibly called on the activeX thread
	activeXJobThread->waitJobsDone();
}

void PresentationEngine_PowerPoint::activateEngine()
{
	activateTimer_.start();

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
			axSSView_->dynamicCall("SetState(int)", set ? (int) Office::PowerPoint::PpSlideShowState::ppSlideShowBlackScreen : (int) Office::PowerPoint::PpSlideShowState::ppSlideShowRunning);
	});
}

void PresentationEngine_PowerPoint::setDisplay(const QRect &rect)
{
	activeXJobThread->executeNonblocking([this, rect]{
		if(!axPresentation_)
			return;

		// Magical constant that makes everything work. Microsoft...
		const double ratio = (3.0/4.0);
		axPresentationWindow_->dynamicCall("SetLeft(double)", (double) rect.left() * ratio);
		axPresentationWindow_->dynamicCall("SetTop(double)", (double) rect.top() * ratio);
		axPresentationWindow_->dynamicCall("SetWidth(double)", (double) rect.width() * ratio);
		axPresentationWindow_->dynamicCall("SetHeight(double)", (double) rect.height() * ratio);
	});
}

void PresentationEngine_PowerPoint::onActivateTimer()
{
	activeXJobThread->executeNonblocking([=]{
		if(axPresentation_)
			axPresentationWindow_->dynamicCall("Activate()");
	});
}
