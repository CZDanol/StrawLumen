#ifndef PRESENTATION_H
#define PRESENTATION_H

#include <QObject>
#include <QSharedPointer>
#include <QPixmap>

class Playlist;
class PresentationEngine;

class Presentation : public QObject
{
	Q_OBJECT

	friend class Playlist;

public:
	static constexpr int maxDescriptionLength = 256;

public:
	virtual ~Presentation() {}

signals:
	void sigSlidesChanged();

public:
	/// Slide count, not considering custom ordering
	virtual int rawSlideCount() const = 0;

	virtual QString rawSlideIdentification(int) const = 0;
	virtual QString rawSlideDescription(int) const = 0;

	/// Slide count considering custom ordering
	int slideCount() const;

	QString slideIdentification(int i) const;
	QString slideDescription(int i) const;

	Playlist *playlist() const;
	int positionInPlaylist() const;

	/// Offset of the slides in the playlist (globalSlideId of the first slide of this presentation)
	int globalSlideIdOffset() const;

public:
	virtual QString identification() const = 0;
	virtual QPixmap icon() const = 0;

	virtual QWidget *createPropertiesWidget(QWidget *parent) = 0;

public:
	virtual PresentationEngine *engine() const = 0;

public:
	/// DO NOT CALL, ONLY TO BE CALLED BY PRESENTATIONMANAGER
	virtual void activatePresentation(int startingSlide) = 0;
	/// DO NOT CALL, ONLY TO BE CALLED BY PRESENTATIONMANAGER
	virtual void deactivatePresentation() = 0;
	/// DO NOT CALL, ONLY TO BE CALLED BY PRESENTATIONMANAGER
	virtual void setSlide(int localSlideId) = 0;

protected:
	Presentation() {}

	void initDefaultSlideOrder();

protected:
	QVector<int> slideOrder_;

private:
	Playlist *playlist_ = nullptr;
	int positionInPlaylist_ = -1;
	int globalSlideIdOffset_ = -1;

};

Q_DECLARE_METATYPE(QSharedPointer<Presentation>)

#endif // PRESENTATION_H
