#include "presentationengine_video.h"

#include "videoprojectorwindow.h"
#include "job/settings.h"

PresentationEngine_Video *presentationEngine_Video = nullptr;

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
}

void PresentationEngine_Video::deactivateEngine()
{
	videoProjectorWindow->close();
}

void PresentationEngine_Video::setBlackScreen(bool set)
{
	if(isBlackScreen_ == set)
		return;

	isBlackScreen_ = set;
	/// TODO
}

void PresentationEngine_Video::setDisplay(const QRect &rect)
{
	videoProjectorWindow->setGeometry(rect);
}
