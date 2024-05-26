#ifndef PRESENTATIONENGINE_VIDEO_H
#define PRESENTATIONENGINE_VIDEO_H

#include "presentation/presentationengine.h"

class PresentationEngine_Video : public PresentationEngine {
	Q_OBJECT

public:
	explicit PresentationEngine_Video(QObject *parent = nullptr);
	~PresentationEngine_Video() override;

public:
	virtual void activateEngine() override;
	virtual void deactivateEngine() override;

	virtual void setBlackScreen(bool set) override;
	virtual void setDisplay(QScreen *screen) override;
	virtual void raiseWindow() override;
};

extern PresentationEngine_Video *presentationEngine_video;

#endif// PRESENTATIONENGINE_VIDEO_H
