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
	activeXJobThread->executeBlocking([=]{
		axApplication_ ->dynamicCall("Quit()");
		delete axApplication_;
	});
}

void PresentationEngine_PowerPoint::activateEngine()
{
	activateTimer_.start();

	if(isInitialized_)
		return;

	isInitialized_ = true;

	activeXJobThread->executeNonblocking([&]{
		axApplication_ = new QAxObject();

		if(!axApplication_->setControl("PowerPoint.Application"))
			return standardErrorDialog(tr("Nepodařilo se spustit prezentaci PowerPoint: instalace programu PowerPoint nenalazena."));

		//connect(axApplication_, SIGNAL(signal(QString, int, void*)), this, SLOT(onPPEvent(QString)));

		axPresentations_ = axApplication_->querySubObject("Presentations");
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

void PresentationEngine_PowerPoint::onActivateTimer()
{
	activeXJobThread->executeNonblocking([=]{
		if(axPresentation_)
			axPresentationWindow_->dynamicCall("Activate()");
	});
}
