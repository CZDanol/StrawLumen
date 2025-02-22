#include "presentationengine_web.h"

#include "job/settings.h"
#include "webprojectorwindow.h"

PresentationEngine_Web *presentationEngine_web = nullptr;

PresentationEngine_Web::PresentationEngine_Web(QObject *parent) : PresentationEngine(parent) {
}

PresentationEngine_Web::~PresentationEngine_Web() {
}

void PresentationEngine_Web::activateEngine() {
	webProjectorWindow->setGeometry(settings->projectionDisplayGeometry());
	webProjectorWindow->show();
}

void PresentationEngine_Web::deactivateEngine() {
	webProjectorWindow->close();
}

void PresentationEngine_Web::setBlackScreen(bool set) {
	webProjectorWindow->setBlackScreen(set);
}

void PresentationEngine_Web::setDisplay(QScreen *screen) {
	webProjectorWindow->setGeometry(screen->geometry());
}

void PresentationEngine_Web::raiseWindow() {
	webProjectorWindow->raise();
}
