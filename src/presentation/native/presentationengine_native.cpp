#include "presentationengine_native.h"

#include "gui/projectorwindow.h"
#include "job/settings.h"

PresentationEngine_Native *presentationEngine_Native = nullptr;

PresentationEngine_Native::PresentationEngine_Native(QObject *parent) : PresentationEngine(parent)
{

}

PresentationEngine_Native::~PresentationEngine_Native()
{

}

void PresentationEngine_Native::setPresentation(const QSharedPointer<Presentation_NativePresentation> &presentation)
{
	if(presentation == currentPresentation_)
		return;

	currentPresentation_ = presentation;
	projectorWindow->update();
}

void PresentationEngine_Native::setSlide(int localSlideId)
{
	if(localSlideId == currentSlide_)
		return;

	currentSlide_ = localSlideId;
	projectorWindow->update();
}

const QSharedPointer<Presentation_NativePresentation> &PresentationEngine_Native::currentPresentation() const
{
	return currentPresentation_;
}

int PresentationEngine_Native::currentSlide() const
{
	return currentPresentation_ ? currentSlide_ : -1;
}

void PresentationEngine_Native::activateEngine()
{
	projectorWindow->setGeometry(settings->projectionDisplayGeometry());
	projectorWindow->show();
}

void PresentationEngine_Native::deactivateEngine()
{
	projectorWindow->close();
}

void PresentationEngine_Native::setBlackScreen(bool set)
{
	if(isBlackScreen_ == set)
		return;

	isBlackScreen_ = set;
	// TODO
}

void PresentationEngine_Native::setDisplay(const QRect &rect)
{
	projectorWindow->setGeometry(rect);
}
