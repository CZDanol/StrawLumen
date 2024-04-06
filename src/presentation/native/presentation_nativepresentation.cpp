#include "presentation_nativepresentation.h"

#include "presentationengine_native.h"

Presentation_NativePresentation::Presentation_NativePresentation() {
}

Presentation_NativePresentation::~Presentation_NativePresentation() {
}

bool Presentation_NativePresentation::isActive() const {
	return isActive_;
}

PresentationEngine *Presentation_NativePresentation::engine() const {
	return presentationEngine_native;
}

void Presentation_NativePresentation::activatePresentation(int startingSlide) {
	presentationEngine_native->setPresentation(weakPtr_);
	presentationEngine_native->setSlide(startingSlide);

	isActive_ = true;
}

void Presentation_NativePresentation::deactivatePresentation() {
	isActive_ = false;
}

void Presentation_NativePresentation::setSlide(int localSlideId, bool force) {
	presentationEngine_native->setSlide(localSlideId, force);
}
