#ifndef PRESENTATIONENGINE_NATIVE_H
#define PRESENTATIONENGINE_NATIVE_H

#include "presentation/presentationengine.h"
#include "presentation/native/presentation_nativepresentation.h"

class PresentationEngine_Native : public PresentationEngine
{
	Q_OBJECT

public:
	explicit PresentationEngine_Native(QObject *parent = nullptr);
	~PresentationEngine_Native();

public slots:
	void setPresentation(const QSharedPointer<Presentation_NativePresentation> &presentation);
	void setSlide(int localSlideId, bool force = false);

public:
	const QSharedPointer<Presentation_NativePresentation> &currentPresentation() const;
	int currentSlide() const;

public:
	void activateEngine() override;
	void deactivateEngine() override;

	void setBlackScreen(bool set) override;
	void setDisplay(const QRect &rect) override;

private:
	bool isBlackScreen_ = false;
	QSharedPointer<Presentation_NativePresentation> currentPresentation_;
	int currentSlide_ = -1;

};

extern PresentationEngine_Native *presentationEngine_Native;

#endif // PRESENTATIONENGINE_NATIVE_H
