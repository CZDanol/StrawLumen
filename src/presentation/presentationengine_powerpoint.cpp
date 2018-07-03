#include "presentationengine_powerpoint.h"

#include "util/standarddialogs.h"
#include "job/activexjobthread.h"

PresentationEngine_PowerPoint *presentationEngine_PowerPoint = nullptr;

PresentationEngine_PowerPoint::PresentationEngine_PowerPoint(QObject *parent) : PresentationEngine(parent)
{
	activateTimer_.setInterval(500);
	connect(&activateTimer_, SIGNAL(timeout()), this, SLOT(onActivateTimer()));
}

PresentationEngine_PowerPoint::~PresentationEngine_PowerPoint()
{
	activeXJobThread->executeBlocking([=]{
		axApplication_ ->dynamicCall("Quit()");
		delete axApplication_;
	});
}

bool PresentationEngine_PowerPoint::activateEngine()
{
	activateTimer_.start();

	if(isInitialized_)
		return isValid_;

	isInitialized_ = true;
	bool result = false;

	activeXJobThread->executeBlocking([&]{
		axApplication_ = new QAxObject();

		if(!axApplication_->setControl("PowerPoint.Application"))
			return standardErrorDialog(tr("Nepodařilo se spustit prezentaci PowerPoint: instalace programu PowerPoint nenalazena."));

		//connect(axApplication_, SIGNAL(signal(QString, int, void*)), this, SLOT(onPPEvent(QString)));

		axPresentations_ = axApplication_->querySubObject("Presentations");

		result = true;
	});

	isValid_ = result;
	return result;
}

void PresentationEngine_PowerPoint::deactivateEngine()
{
	activateTimer_.stop();
}

void PresentationEngine_PowerPoint::onActivateTimer()
{
	activeXJobThread->executeNonblocking([=]{
		if(axPresentation_)
			axPresentationWindow_->dynamicCall("Activate()");
	});
}
