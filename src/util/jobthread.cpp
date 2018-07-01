#include "jobthread.h"

#include <QtConcurrent/QtConcurrent>
#include <QMutexLocker>

JobThread::JobThread()
{
	thread_ = std::thread([=]{threadFunction();});
}

JobThread::~JobThread()
{
	QMutexLocker ml(&mutex_);
	doQuit_ = true;
	wakeCondition_.wakeAll();
	quitCondition_.wait(&mutex_);

	thread_.join();
}

void JobThread::executeNonblocking(JobThread::Job job)
{
	QMutexLocker ml(&mutex_);
	jobQueue_.enqueue(job);
	wakeCondition_.wakeOne();
}

void JobThread::executeBlocking(JobThread::Job job)
{
	QMutex m;
	QWaitCondition c;

	QMutexLocker ml(&m);
	executeNonblocking([&] {
		job();

		QMutexLocker ml(&m);
		c.wakeOne();
	});

	c.wait(&m);
}

void JobThread::threadFunction()
{
	mutex_.lock();

	while(true) {
		if( jobQueue_.empty() && doQuit_ ) {
			quitCondition_.wakeOne();
			mutex_.unlock();
			return;

		} else if( jobQueue_.empty() ) {
			wakeCondition_.wait(&mutex_);
			continue;

		}

		Job job = jobQueue_.dequeue();
		mutex_.unlock();

		job();

		mutex_.lock();
	}

}
