#ifndef EXECONMAINTHREAD_H
#define EXECONMAINTHREAD_H

#include <functional>

#include <QObject>

using _ExecOnMainThreadJob = std::function<void()>;

class _ExecOnMainThreadUtility : public QObject {
	Q_OBJECT

public:
	_ExecOnMainThreadUtility();

public slots:
	void onJob(const _ExecOnMainThreadJob &job);

};

Q_DECLARE_METATYPE(_ExecOnMainThreadJob)

void execOnMainThread(const _ExecOnMainThreadJob &job);

#endif // EXECONMAINTHREAD_H
