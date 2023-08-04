#include "presentationengine_powerpoint.h"

#include <QApplication>
#include <QDebug>

#include "gui/mainwindow.h"
#include "job/activexjobthread.h"
#include "presentation/presentationmanager.h"
#include "presentation/powerpoint/presentation_powerpoint.h"

PresentationEngine_PowerPoint *presentationEngine_powerPoint = nullptr;

PresentationEngine_PowerPoint::PresentationEngine_PowerPoint(QObject *parent) : PresentationEngine(parent)
{
    activateTimer_.setInterval(1000);
    connect(&activateTimer_, SIGNAL(timeout()), this, SLOT(onActivateTimer()));
}

PresentationEngine_PowerPoint::~PresentationEngine_PowerPoint()
{
    // Prevent destroying the engine before deactivateEngine is possibly called on the activeX thread
    activeXJobThread->waitJobsDone();
}

void PresentationEngine_PowerPoint::activateEngine()
{
#ifndef QT_DEBUG
    // For some reason, the application loses focus when the PPT is activated, but only in debug mode
    activateTimer_.start();
#endif

    if(isInitialized_)
        return;

    isInitialized_ = true;
    dotsPerPoint_ = QPointF(mainWindow->logicalDpiX(), mainWindow->logicalDpiY()) / 72;

    activeXJobThread->executeNonblocking([&]{
        auto axApplication = activeXJobThread->axPowerPointApplication;

        axPresentations_ = axApplication->querySubObject("Presentations");
    });
}

void PresentationEngine_PowerPoint::deactivateEngine()
{
    activateTimer_.stop();
}

void PresentationEngine_PowerPoint::setBlackScreen(bool set)
{
    auto currentPptPresentation = presentationManager->currentPresentation().dynamicCast<Presentation_PowerPoint>();
    if(!currentPptPresentation.isNull() && currentPptPresentation->isSlideBlackScreen(presentationManager->currentLocalSlideId()))
        set = true;

    activeXJobThread->executeNonblocking([=]{
        if(!axPresentation_)
            return;

        axSSView_->dynamicCall("SetState(int)", int(set ? Office::PowerPoint::PpSlideShowState::ppSlideShowBlackScreen : Office::PowerPoint::PpSlideShowState::ppSlideShowRunning));
    });
}

void PresentationEngine_PowerPoint::setDisplay(const QRect &rect)
{
    activeXJobThread->executeNonblocking([this, rect]{
        setDisplay_axThread(rect);
    });
}

void PresentationEngine_PowerPoint::raiseWindow()
{
    // TODO
}

void PresentationEngine_PowerPoint::setDisplay_axThread(const QRect &rect)
{
    if(!axPresentation_)
        return;

    axPresentationWindow_->dynamicCall("SetLeft(double)", double(rect.left() / dotsPerPoint_.x()));
    axPresentationWindow_->dynamicCall("SetTop(double)", double(rect.top() / dotsPerPoint_.y()));
    axPresentationWindow_->dynamicCall("SetWidth(double)", double(rect.width() / dotsPerPoint_.x()));
    axPresentationWindow_->dynamicCall("SetHeight(double)",double(rect.height() / dotsPerPoint_.y()));
}

void PresentationEngine_PowerPoint::onActivateTimer()
{
    activeXJobThread->executeNonblocking([=]{
        if(!axPresentation_)
            return;

        axPresentationWindow_->dynamicCall("Activate()");
    });
}
