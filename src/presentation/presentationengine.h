#ifndef PRESENTATIONENGINE_H
#define PRESENTATIONENGINE_H

#include <QObject>

class PresentationEngine : public QObject
{
	Q_OBJECT

public:
	explicit PresentationEngine(QObject *parent = nullptr);
	virtual ~PresentationEngine() {}

public:
	virtual void activateEngine() = 0;
	virtual void deactivateEngine() = 0;

	virtual void setBlackScreen(bool set) = 0;

};

#endif // PRESENTATIONENGINE_H
