#ifndef PRESENTATIONENGINE_H
#define PRESENTATIONENGINE_H

#include <QObject>
#include <QScreen>

class PresentationEngine : public QObject {
	Q_OBJECT

public:
	explicit PresentationEngine(QObject *parent = nullptr);

	virtual ~PresentationEngine() {}

public:
	virtual void activateEngine() = 0;
	virtual void deactivateEngine() = 0;

	virtual void setBlackScreen(bool set) = 0;
	virtual void setDisplay(QScreen *screen) = 0;
	virtual void raiseWindow() = 0;
};

#endif// PRESENTATIONENGINE_H
