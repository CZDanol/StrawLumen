#include "presentationmanager.h"

#include "presentation/presentation.h"
#include "presentation/presentationengine.h"
#include "rec/playlist.h"

PresentationManager *presentationManager = nullptr;

PresentationManager::PresentationManager(QObject *parent) : QObject(parent) {
}

PresentationManager::~PresentationManager() {
	setPresentation(nullptr);
}

bool PresentationManager::isActive() const {
	return isActive_;
}

PresentationEngine *PresentationManager::currentEngine() const {
	return currentEngine_;
}

QSharedPointer<Presentation> PresentationManager::currentPresentation() const {
	return currentPresentation_;
}

int PresentationManager::currentLocalSlideId() const {
	return currentLocalSlideId_;
}

int PresentationManager::currentGlobalSlideId() const {
	if(currentPresentation_.isNull() || currentLocalSlideId_ == -1)
		return -1;

	return currentPresentation_->globalSlideIdOffset() + currentLocalSlideId_;
}

void PresentationManager::nextSlide() {
	if(currentPresentation_.isNull())
		return;

	setSlide(currentPresentation_->playlist(), currentGlobalSlideId() + 1);
}

void PresentationManager::previousSlide() {
	if(currentPresentation_.isNull())
		return;

	if(currentGlobalSlideId() == 0)
		return;

	setSlide(currentPresentation_->playlist(), currentGlobalSlideId() - 1);
}

void PresentationManager::nextPresentation() {
	if(currentPresentation_.isNull())
		return;

	setSlide(currentPresentation_->playlist(), currentPresentation_->globalSlideIdOffset() + currentPresentation_->slideCount());
}

void PresentationManager::previousPresentation() {
	if(currentPresentation_.isNull())
		return;

	if(currentGlobalSlideId() == 0)
		return;

	if(currentLocalSlideId_ == 0) {
		int i = currentPresentation_->positionInPlaylist() - 1;
		while(currentPresentation_->playlist()->items()[i]->slideCount() == 0)
			i--;

		if(i < 0)
			return;

		setSlide(currentPresentation_->playlist(), currentPresentation_->playlist()->items()[i]->globalSlideIdOffset());
	}
	else
		setSlide(currentPresentation_->playlist(), currentPresentation_->globalSlideIdOffset());
}

void PresentationManager::setSlide(const Playlist *playlist, int globalSlideId, bool force) {
	if(!playlist || playlist->slideCount() == 0)
		return setActive(false);

	if(globalSlideId >= playlist->slideCount())
		globalSlideId = playlist->slideCount() - 1;

	else if(globalSlideId < 0)
		globalSlideId = 0;

	auto presentation = playlist->presentationOfSlide(globalSlideId);
	int localSlideId = globalSlideId - presentation->globalSlideIdOffset();

	setSlide(presentation, localSlideId, force);
}

void PresentationManager::setSlide(const QSharedPointer<Presentation> &presentation, int localSlideId, bool force) {
	if(!presentation || !presentation->slideCount())
		return setActive(false);

	if(localSlideId < 0)
		localSlideId = 0;
	else if(localSlideId >= presentation->slideCount())
		localSlideId = presentation->slideCount() - 1;

	setBlackScreen(false);
	setPresentation(presentation, localSlideId);

	if(force || currentLocalSlideId_ != localSlideId) {
		presentation->setSlide(localSlideId, force);
		currentLocalSlideId_ = localSlideId;
	}

	const int globalSlideId = presentation->globalSlideIdOffset() + localSlideId;

	emit sigCurrentSlideChanged(globalSlideId);

	// Preload neighbor slides
	/*auto playlist = presentation->playlist();

	if(localSlideId > 0)
		presentation->preloadSlide(localSlideId - 1);
	else if(playlist && globalSlideId > 0) {
		auto pres = playlist->presentationOfSlide(globalSlideId-1);
		pres->preloadSlide(pres->slideCount()-1);
	}

	if(localSlideId < presentation->slideCount()-1)
		presentation->preloadSlide(localSlideId+1);
	else if(playlist && globalSlideId < playlist->slideCount()-1)
		playlist->presentationOfSlide(globalSlideId-1)->preloadSlide(0);*/
}

void PresentationManager::setActive(bool set) {
	if(!set)
		return setEngine(nullptr);

	// TODO Turn on black screen
}

void PresentationManager::setBlackScreen(bool set) {
	if(isBlackScren_ == set)
		return;

	isBlackScren_ = set;

	if(currentEngine_)
		currentEngine_->setBlackScreen(set);

	emit sigBlackScreenChanged(set);
}

void PresentationManager::raiseWindow() {
	if(currentEngine_)
		currentEngine_->raiseWindow();
}

void PresentationManager::reinitializeCurrentPresentation() {
	auto presentation = currentPresentation_;
	int localSlideId = currentLocalSlideId_;

	setActive(false);
	setSlide(presentation, localSlideId);
}

void PresentationManager::setPresentation(const QSharedPointer<Presentation> &presentation, int startingSlide) {
	if(currentPresentation_ == presentation)
		return;

	if(!currentPresentation_.isNull()) {
		currentPresentation_->deactivatePresentation();
		currentPresentation_.clear();
		currentLocalSlideId_ = -1;
	}

	if(presentation.isNull())
		return setActive(false);

	setEngine(presentation->engine());
	presentation->activatePresentation(startingSlide);

	// This has to be before the setBlackScreen, because powerpoints setBlackScreen gets data from the current presentation
	currentPresentation_ = presentation;
	currentLocalSlideId_ = startingSlide;

	currentEngine_->setBlackScreen(isBlackScren_);
}

void PresentationManager::setEngine(PresentationEngine *engine) {
	if(engine == currentEngine_)
		return;

	if(!currentPresentation_.isNull()) {
		currentPresentation_->deactivatePresentation();
		currentPresentation_.clear();
		currentLocalSlideId_ = -1;
	}

	if(currentEngine_) {
		currentEngine_->deactivateEngine();
		currentEngine_ = nullptr;
	}

	if(engine) {
		engine->activateEngine();
		engine->setBlackScreen(isBlackScren_);
	}

	currentEngine_ = engine;
	_changeActive(currentEngine_ != nullptr);
}

void PresentationManager::_changeActive(bool set) {
	if(isActive_ == set)
		return;

	isActive_ = set;
	emit sigActiveChanged(set);
}
