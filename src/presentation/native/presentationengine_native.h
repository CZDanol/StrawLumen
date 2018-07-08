#ifndef PRESENTATIONENGINE_NATIVE_H
#define PRESENTATIONENGINE_NATIVE_H

#include "presentation/presentationengine.h"

class PresentationEngine_Native : public PresentationEngine
{
	Q_OBJECT

public:
	explicit PresentationEngine_Native(QObject *parent = nullptr);
	~PresentationEngine_Native();

public:
	void activateEngine() override;
	void deactivateEngine() override;

	void setBlackScreen(bool set) override;
	void setDisplay(const QRect &rect) override;

private:
	bool isBlackScreen_ = false;

};

extern PresentationEngine_Native *presentationEngine_Native;

#endif // PRESENTATIONENGINE_NATIVE_H
