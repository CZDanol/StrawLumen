#include "presentationengine_native.h"

#include "gui/projectorwindow.h"
#include "gui/settingsdialog.h"

PresentationEngine_Native *presentationEngine_Native = nullptr;

PresentationEngine_Native::PresentationEngine_Native(QObject *parent) : PresentationEngine(parent)
{

}

PresentationEngine_Native::~PresentationEngine_Native()
{

}

void PresentationEngine_Native::activateEngine()
{
	projectorWindow->setGeometry(settingsDialog->projectionDisplayGeometry());
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
