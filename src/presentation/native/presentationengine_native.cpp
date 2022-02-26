#include "presentationengine_native.h"

#include "presentation/native/nativeprojectorwindow.h"
#include "job/settings.h"

PresentationEngine_Native *presentationEngine_native = nullptr;

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
	nativeProjectorWindow->update();
}

void PresentationEngine_Native::setSlide(int localSlideId, bool force)
{
	if(!force && localSlideId == currentSlide_)
		return;

	currentSlide_ = localSlideId;
	nativeProjectorWindow->update();
}

const QSharedPointer<Presentation_NativePresentation> &PresentationEngine_Native::currentPresentation() const
{
	return currentPresentation_;
}

int PresentationEngine_Native::currentSlide() const
{
	return currentPresentation_ ? currentSlide_ : -1;
}

bool PresentationEngine_Native::isBlackScreen() const
{
	return isBlackScreen_;
}

void PresentationEngine_Native::activateEngine()
{
	nativeProjectorWindow->setGeometry(settings->projectionDisplayGeometry());
	nativeProjectorWindow->show();
}

void PresentationEngine_Native::deactivateEngine()
{
	nativeProjectorWindow->close();
}

void PresentationEngine_Native::setBlackScreen(bool set)
{
	if(isBlackScreen_ == set)
		return;

	isBlackScreen_ = set;
	nativeProjectorWindow->update();
}

void PresentationEngine_Native::setDisplay(const QRect &rect)
{
	nativeProjectorWindow->setGeometry(rect);
}

void PresentationEngine_Native::raiseWindow()
{
	nativeProjectorWindow->raise();
}
