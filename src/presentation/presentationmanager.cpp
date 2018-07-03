#include "presentationmanager.h"

#include "presentation/presentationengine.h"
#include "presentation/presentation.h"

PresentationManager *presentationManager = nullptr;

PresentationManager::PresentationManager(QObject *parent) : QObject(parent)
{

}

PresentationManager::~PresentationManager()
{
	setPresentation(nullptr);
}

QSharedPointer<Presentation> PresentationManager::currentPresentation() const
{
	return currentPresentation_;
}

int PresentationManager::currentSlideId() const
{
	return currentSlideId_;
}

bool PresentationManager::setPresentation(const QSharedPointer<Presentation> &presentation)
{
	if(currentPresentation_ == presentation)
		return true;

	if(!currentPresentation_.isNull()) {
		currentPresentation_->deactivatePresentation();
		currentPresentation_.clear();
		currentSlideId_ = -1;
	}

	if(presentation.isNull()) {
		setEngine(nullptr);
		return true;
	}

	if(!setEngine(presentation->engine()))
		return false;

	if(!presentation->activatePresentation()) {
		setEngine(nullptr);
		return false;
	}

	currentPresentation_ = presentation;
	currentSlideId_ = 0;

	return true;
}

bool PresentationManager::setEngine(PresentationEngine *engine)
{
	if(engine == currentEngine_)
		return true;

	if(currentEngine_) {
		currentEngine_->deactivateEngine();
		currentEngine_ = nullptr;
	}

	currentPresentation_.clear();
	currentSlideId_ = -1;

	if(engine && !engine->activateEngine())
		return false;

	currentEngine_ = engine;

	return true;
}
