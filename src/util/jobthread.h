#ifndef JOBTHREAD_H
#define JOBTHREAD_H

#include <functional>
#include <thread>

#include <QQueue>
#include <QMutex>
#include <QWaitCondition>

class JobThread
{

public:
	using Job = std::function<void()>;

public:
	JobThread();
	virtual ~JobThread();

public:
	void executeNonblocking(Job job);
	void executeBlocking(Job job);

private:
	void threadFunction();

private:
	QQueue<Job> jobQueue_;
	QWaitCondition wakeCondition_, quitCondition_;
	QMutex mutex_;
	bool doQuit_ = false;
	std::thread thread_;

};

#endif // JOBTHREAD_H
