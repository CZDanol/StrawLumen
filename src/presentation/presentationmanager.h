#ifndef PRESENTATIONMANAGER_H
#define PRESENTATIONMANAGER_H

#include <QObject>
#include <QSharedPointer>

class Presentation;
class PresentationEngine;
class Playlist;

class PresentationManager : public QObject {
	Q_OBJECT

public:
	explicit PresentationManager(QObject *parent = nullptr);
	~PresentationManager();

public:
	bool isActive() const;
	PresentationEngine *currentEngine() const;
	QSharedPointer<Presentation> currentPresentation() const;
	int currentLocalSlideId() const;
	int currentGlobalSlideId() const;

public slots:
	void setSlide(const Playlist *playlist, int globalSlideId, bool force = false);
	void setSlide(const QSharedPointer<Presentation> &presentation, int localSlideId, bool force = false);

	void nextSlide();
	void previousSlide();
	void nextPresentation();
	void previousPresentation();

	void setActive(bool set);
	void setBlackScreen(bool set);

	void raiseWindow();

	void reinitializeCurrentPresentation();

signals:
	void sigActiveChanged(bool newValue);
	void sigCurrentSlideChanged(int globalSlideId);
	void sigBlackScreenChanged(bool isOn);

private:
	void setPresentation(const QSharedPointer<Presentation> &presentation, int startingSlide = 0);
	void setEngine(PresentationEngine *engine);

private:
	void _changeActive(bool set);

private:
	bool isActive_ = false;
	PresentationEngine *currentEngine_ = nullptr;
	QSharedPointer<Presentation> currentPresentation_;
	int currentLocalSlideId_ = -1;
	bool isBlackScren_ = false;
};

extern PresentationManager *presentationManager;

#endif// PRESENTATIONMANAGER_H
