#ifndef ACTIVEXJOBTHREAD_H
#define ACTIVEXJOBTHREAD_H

#include <QObject>
#include <QAxObject>

#include "util/jobthread.h"

namespace Office {

	enum class MsoTriState : int {
		msoTrue = -1,
		msoFalse = 0
	};

	namespace PowerPoint {

		enum class PpSlideShowType : int {
			ppShowTypeKiosk	= 3,
			ppShowTypeSpeaker = 1,
			ppShowTypeWindow = 2
		};

		enum class PpSlideShowAdvanceMode  : int {
			ppSlideShowManualAdvance = 1,
			ppSlideShowRehearseNewTimings = 3,
			ppSlideShowUseSlideTimings = 2
		};

		enum class PpSlideShowState : int {
			ppSlideShowBlackScreen = 3,
			ppSlideShowDone = 5,
			ppSlideShowPaused = 2,
			ppSlideShowRunning = 1,
			ppSlideShowWhiteScreen = 4
		};

		enum class PpSlideShowRangeType  : int {
			ppShowAll = 1,
			ppShowNamedSlideShow = 3,
			ppShowSlideRange = 2
		};

	}
}

class ActiveXJobThread : public JobThread {
	Q_OBJECT

public:
	explicit ActiveXJobThread(QObject *parent = nullptr);
	~ActiveXJobThread();

public:
	QAxObject *axPowerPointApplication = nullptr;

};

extern ActiveXJobThread *activeXJobThread;

#endif // ACTIVEXJOBTHREAD_H
