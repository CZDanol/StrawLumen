#include "presentationengine_video.h"

#include "videoprojectorwindow.h"
#include "job/settings.h"
#include "gui/mainwindow.h"
#include "gui/mainwindow_presentationmode.h"

PresentationEngine_Video *presentationEngine_video = nullptr;

PresentationEngine_Video::PresentationEngine_Video(QObject *parent) : PresentationEngine(parent)
{

}

PresentationEngine_Video::~PresentationEngine_Video()
{

}

void PresentationEngine_Video::activateEngine()
{
	videoProjectorWindow->setGeometry(settings->projectionDisplayGeometry());
	videoProjectorWindow->show();

	mainWindow->presentationMode()->videoControlWidget()->show();
}

void PresentationEngine_Video::deactivateEngine()
{
	videoProjectorWindow->close();
	mainWindow->presentationMode()->videoControlWidget()->hide();
}

void PresentationEngine_Video::setBlackScreen(bool set)
{
	videoProjectorWindow->setBlackScreen(set);
}

void PresentationEngine_Video::setDisplay(const QRect &rect)
{
	videoProjectorWindow->setGeometry(rect);
}
