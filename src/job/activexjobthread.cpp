#include "activexjobthread.h"

#include "util/standarddialogs.h"

#include <windows.h>

ActiveXJobThread *activeXJobThread = nullptr;

ActiveXJobThread::ActiveXJobThread(QObject *parent) : JobThread(parent)
{
	qRegisterMetaType<int>("Office::MsoTriState");

	executeNonblocking([this]{
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

		axPowerPointApplication = new QAxObject();

		if(!axPowerPointApplication->setControl("PowerPoint.Application")) {
			delete axPowerPointApplication;
			axPowerPointApplication = nullptr;
		}
	});
}

ActiveXJobThread::~ActiveXJobThread()
{
	executeBlocking([this]{
		if(axPowerPointApplication) {
			axPowerPointApplication->dynamicCall("Quit()");
			delete axPowerPointApplication;
			axPowerPointApplication = nullptr;
		}

		CoUninitialize();
	});
}
