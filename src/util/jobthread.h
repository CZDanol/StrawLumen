#ifndef JOBTHREAD_H
#define JOBTHREAD_H

#include <functional>
#include <thread>

#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QWaitCondition>

class JobThread : public QObject {
	Q_OBJECT

public:
	using Job = std::function<void()>;

public:
	explicit JobThread(QObject *parent = nullptr);
	virtual ~JobThread();

public:
	void executeNonblocking(Job job);
	void executeBlocking(Job job);

	/// Waits until all jobs currently in the queue are done
	void waitJobsDone();

private:
	void threadFunction();

private:
	QQueue<Job> jobQueue_;
	QWaitCondition wakeCondition_, quitCondition_;
	QMutex mutex_;
	bool doQuit_ = false;
	std::thread thread_;
};

#endif// JOBTHREAD_H
