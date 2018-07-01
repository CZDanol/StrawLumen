#include "activexjobthread.h"

#include <windows.h>

ActiveXJobThread *activeXJobThread = nullptr;

ActiveXJobThread::ActiveXJobThread()
{
	qRegisterMetaType<int>("Office::MsoTriState");

	executeNonblocking([]{
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	});
}

ActiveXJobThread::~ActiveXJobThread()
{
	executeNonblocking([]{
		CoUninitialize();
	});
}
