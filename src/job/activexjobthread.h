#ifndef ACTIVEXJOBTHREAD_H
#define ACTIVEXJOBTHREAD_H

#include <QObject>

#include "util/jobthread.h"

namespace Office {
	enum class MsoTriState : int {
		msoTrue = -1,
		msoFalse = 0
	};
}

class ActiveXJobThread : public JobThread {

public:
	ActiveXJobThread();
	~ActiveXJobThread();

};

extern ActiveXJobThread *activeXJobThread;

#endif // ACTIVEXJOBTHREAD_H
