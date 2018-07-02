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
	virtual bool activateEngine() = 0;
	virtual void deactivateEngine() = 0;

};

#endif // PRESENTATIONENGINE_H
