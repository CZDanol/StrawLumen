#ifndef PRESENTATIONMANAGER_H
#define PRESENTATIONMANAGER_H

#include <QObject>
#include <QSharedPointer>

class Presentation;
class PresentationEngine;
class Playlist;

class PresentationManager : public QObject
{
	Q_OBJECT

public:
	explicit PresentationManager(QObject *parent = nullptr);
	~PresentationManager();

public:
	QSharedPointer<Presentation> currentPresentation() const;
	int currentLocalSlideId() const;
	int currentGlobalSlideId() const;

public slots:
	bool nextSlide();
	bool setSlide(const Playlist *playlist, int globalSlideId);

	bool setPresentation(const QSharedPointer<Presentation> &presentation);
	bool setEngine(PresentationEngine *engine);
	bool setActive(bool set);

signals:
	void sigActiveChanged(bool newValue);
	void sigCurrentSlideChanged();

private:
	void _changeActive(bool set);

private:
	bool isActive_ = false;
	PresentationEngine *currentEngine_ = nullptr;
	QSharedPointer<Presentation> currentPresentation_;
	int currentLocalSlideId_ = -1;

};

extern PresentationManager *presentationManager;

#endif // PRESENTATIONMANAGER_H
