#ifndef PRESENTATION_NATIVEPRESENTATION_H
#define PRESENTATION_NATIVEPRESENTATION_H

#include "presentation/presentation.h"

class Presentation_NativePresentation : public Presentation
{
	Q_OBJECT

public:
	virtual ~Presentation_NativePresentation();

public:
	PresentationEngine *engine() const override;

	void activatePresentation(int startingSlide) override;
	void deactivatePresentation() override;
	void setSlide(int localSlideId) override;

protected:
	Presentation_NativePresentation();

};

#endif // PRESENTATION_NATIVEPRESENTATION_H
