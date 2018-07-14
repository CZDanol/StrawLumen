#include "execonmainthread.h"

#include <QTimer>

#include "gui/mainwindow.h"

void execOnMainThread(const std::function<void()> &job)
{
	QTimer::singleShot(0, mainWindow, job);
}
