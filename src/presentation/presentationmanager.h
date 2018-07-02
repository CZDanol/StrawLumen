#ifndef PRESENTATIONMANAGER_H
#define PRESENTATIONMANAGER_H

#include <QObject>
#include <QSharedPointer>

class Presentation;
class PresentationEngine;

class PresentationManager : public QObject
{
	Q_OBJECT

public:
	explicit PresentationManager(QObject *parent = nullptr);
	~PresentationManager();

public:
	QSharedPointer<Presentation> currentPresentation() const;
	int currentSlideId() const;

public:
	bool setPresentation(const QSharedPointer<Presentation> &presentation);
	bool setEngine(PresentationEngine *engine);

private:
	PresentationEngine *currentEngine_ = nullptr;
	QSharedPointer<Presentation> currentPresentation_;
	int currentSlideId_ = -1;

};

extern PresentationManager *presentationManager;

#endif // PRESENTATIONMANAGER_H
