#include "execonmainthread.h"

static _ExecOnMainThreadUtility _execOnMainThreadUtility;

void execOnMainThread(const _ExecOnMainThreadJob &job) {
	QMetaObject::invokeMethod(&_execOnMainThreadUtility, "onJob", Qt::QueuedConnection, Q_ARG(_ExecOnMainThreadJob, job));
}

_ExecOnMainThreadUtility::_ExecOnMainThreadUtility() {
	qRegisterMetaType<_ExecOnMainThreadJob>();
}

void _ExecOnMainThreadUtility::onJob(const _ExecOnMainThreadJob &job) {
	job();
}
