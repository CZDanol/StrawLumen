#include "presentationengine_powerpoint.h"

#include <QApplication>
#include <QDebug>

#include "job/activexjobthread.h"
#include "presentation/powerpoint/presentation_powerpoint.h"
#include "presentation/presentationmanager.h"

PresentationEngine_PowerPoint *presentationEngine_powerPoint = nullptr;

PresentationEngine_PowerPoint::PresentationEngine_PowerPoint(QObject *parent) : PresentationEngine(parent) {
	activateTimer_.setInterval(1000);
	connect(&activateTimer_, SIGNAL(timeout()), this, SLOT(onActivateTimer()));
}

PresentationEngine_PowerPoint::~PresentationEngine_PowerPoint() {
	// Prevent destroying the engine before deactivateEngine is possibly called on the activeX thread
	activeXJobThread->waitJobsDone();
}

void PresentationEngine_PowerPoint::activateEngine() {
#ifndef QT_DEBUG
	// For some reason, the application loses focus when the PPT is activated, but only in debug mode
	activateTimer_.start();
#endif

	if(isInitialized_)
		return;

	isInitialized_ = true;

	activeXJobThread->executeNonblocking([&] {
		auto axApplication = activeXJobThread->axPowerPointApplication;

		axPresentations_ = axApplication->querySubObject("Presentations");
		if(!axPresentations_) {
			qWarning() << "Presentations null";
		}
	});
}

void PresentationEngine_PowerPoint::deactivateEngine() {
	activateTimer_.stop();
}

void PresentationEngine_PowerPoint::setBlackScreen(bool set) {
	auto currentPptPresentation = presentationManager->currentPresentation().dynamicCast<Presentation_PowerPoint>();
	if(!currentPptPresentation.isNull() && currentPptPresentation->isSlideBlackScreen(presentationManager->currentLocalSlideId()))
		set = true;

	activeXJobThread->executeNonblocking([=] {
		auto view = presentation_.axSSView_;
		if(!view)
			return;

		qDebug() << "SetState";
		view->dynamicCall("SetState(int)", int(set ? Office::PowerPoint::PpSlideShowState::ppSlideShowBlackScreen : Office::PowerPoint::PpSlideShowState::ppSlideShowRunning));
	});
}

void PresentationEngine_PowerPoint::setDisplay(QScreen *screen) {
	// PowerPoint is using some really weird coordinate mechanics... :/
	const float commonCoef = 1.0f / qApp->primaryScreen()->devicePixelRatio();
	const float posCoef = 72.0f / 96.0f * commonCoef;
	const float sizeCoef = screen->devicePixelRatio() * posCoef;
	const auto geom = screen->geometry().toRectF();
	const QRectF rect(geom.left() * posCoef, geom.top() * posCoef, geom.width() * sizeCoef, geom.height() * sizeCoef);

	activeXJobThread->executeNonblocking([this, rect] {
		setDisplay_axThread(rect);
	});
}

void PresentationEngine_PowerPoint::raiseWindow() {
	// TODO
}

void PresentationEngine_PowerPoint::setDisplay_axThread(const QRectF &rect) {
	qDebug() << "SetDisplay";

	auto wnd = presentation_.axPresentationWindow_;
	if(!wnd)
		return;

	wnd->dynamicCall("SetLeft(double)", rect.left());
	wnd->dynamicCall("SetTop(double)", rect.top());
	wnd->dynamicCall("SetWidth(double)", rect.width());
	wnd->dynamicCall("SetHeight(double)", rect.height());
}

void PresentationEngine_PowerPoint::onActivateTimer() {
	activeXJobThread->executeNonblocking([=] {
		auto wnd = presentation_.axPresentationWindow_;
		if(!wnd)
			return;

		qDebug() << "Activate";
		wnd->dynamicCall("Activate()");
	});
}

void PresentationEngine_PowerPoint::PresentationData::reset()
{
	qDebug() << "Reset";

	if(axPresentation_) {
		axPresentation_->dynamicCall("Close()");
		delete axPresentation_;
	}

	*this = {};
}
