#include "presentationmanager.h"

#include "presentation/presentationengine.h"
#include "presentation/presentation.h"
#include "presentation/playlist.h"

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

int PresentationManager::currentLocalSlideId() const
{
	return currentLocalSlideId_;
}

int PresentationManager::currentGlobalSlideId() const
{
	if(currentPresentation_.isNull() || currentLocalSlideId_ == -1)
		return -1;

	return currentPresentation_->globalSlideIdOffset() + currentLocalSlideId_;
}

bool PresentationManager::nextSlide()
{
	if(currentPresentation_.isNull())
		return false;

	return setSlide(currentPresentation_->playlist(), currentGlobalSlideId() + 1);
}

bool PresentationManager::setSlide(const Playlist* playlist, int globalSlideId)
{
	if( globalSlideId < 0 || globalSlideId >= playlist->slideCount() )
		setPresentation(nullptr); // TODO black screen instead

	auto presentation = playlist->presentationOfSlide(globalSlideId);
	int localSlideId = globalSlideId - presentation->globalSlideIdOffset();

	if( !setPresentation(presentation) )
		return false;

	if( !presentation->setSlide(localSlideId) ) {
		setActive(false);
		return false;
	}

	emit sigCurrentSlideChanged();
	return true;
}

bool PresentationManager::setPresentation(const QSharedPointer<Presentation> &presentation)
{
	if(currentPresentation_ == presentation)
		return true;

	if(!currentPresentation_.isNull()) {
		currentPresentation_->deactivatePresentation();
		currentPresentation_.clear();
		currentLocalSlideId_ = -1;
	}

	if(presentation.isNull()) {
		setActive(false);
		return true;
	}

	if(!setEngine(presentation->engine()))
		return false;

	if(!presentation->activatePresentation()) {
		setActive(false);
		return false;
	}

	currentPresentation_ = presentation;
	currentLocalSlideId_ = -1;

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
	currentLocalSlideId_ = -1;

	if(engine && !engine->activateEngine()) {
		_changeActive(false);
		return false;
	}

	currentEngine_ = engine;
	_changeActive(currentEngine_ != nullptr);

	return true;
}

bool PresentationManager::setActive(bool set)
{
	if(!set)
		return setEngine(nullptr);

	// TODO Turn on on black screen
	else
		return false;
}

void PresentationManager::_changeActive(bool set)
{
	if(isActive_ == set)
		return;

	isActive_ = set;
	emit sigActiveChanged(set);
}
