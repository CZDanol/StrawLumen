#ifndef PRESENTATION_NATIVEPRESENTATION_H
#define PRESENTATION_NATIVEPRESENTATION_H

#include <QPainter>
#include <QRect>

#include "presentation/presentation.h"

class Presentation_NativePresentation : public Presentation {
	Q_OBJECT

public:
	virtual ~Presentation_NativePresentation() override;

public:
	virtual void drawSlide(QPainter &p, int slideId, const QRect &rect) = 0;

public:
	bool isActive() const;

	PresentationEngine *engine() const override;

	virtual void activatePresentation(int startingSlide) override;
	virtual void deactivatePresentation() override;
	void setSlide(int localSlideId, bool force = false) override;

protected:
	Presentation_NativePresentation();

protected:
	QWeakPointer<Presentation_NativePresentation> weakPtr_;

private:
	bool isActive_ = false;
};

#endif// PRESENTATION_NATIVEPRESENTATION_H
