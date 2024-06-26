#include "presentationengine_video.h"

#include "gui/mainwindow.h"
#include "gui/mainwindow_presentationmode.h"
#include "job/settings.h"
#include "videoprojectorwindow.h"

PresentationEngine_Video *presentationEngine_video = nullptr;

PresentationEngine_Video::PresentationEngine_Video(QObject *parent) : PresentationEngine(parent) {
}

PresentationEngine_Video::~PresentationEngine_Video() {
}

void PresentationEngine_Video::activateEngine() {
	videoProjectorWindow->setGeometry(settings->projectionDisplayGeometry());
	videoProjectorWindow->show();

	mainWindow->presentationMode()->videoControlWidget()->show();
}

void PresentationEngine_Video::deactivateEngine() {
	videoProjectorWindow->close();
	mainWindow->presentationMode()->videoControlWidget()->hide();
}

void PresentationEngine_Video::setBlackScreen(bool set) {
	videoProjectorWindow->setBlackScreen(set);
}

void PresentationEngine_Video::setDisplay(QScreen *screen) {
	videoProjectorWindow->setGeometry(screen->geometry());
}

void PresentationEngine_Video::raiseWindow() {
	videoProjectorWindow->raise();
}
