#ifndef PRESENTATIONENGINE_VIDEO_H
#define PRESENTATIONENGINE_VIDEO_H

#include "presentation/presentationengine.h"

class PresentationEngine_Video : public PresentationEngine
{
	Q_OBJECT

public:
	explicit PresentationEngine_Video(QObject *parent =  nullptr);
	~PresentationEngine_Video() override;

public:
	void activateEngine() override;
	void deactivateEngine() override;

	void setBlackScreen(bool set) override;
	void setDisplay(const QRect &rect) override;

};

extern PresentationEngine_Video *presentationEngine_video;

#endif // PRESENTATIONENGINE_VIDEO_H
