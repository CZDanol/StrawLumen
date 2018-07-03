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

public:
	/// Slide count, not considering custom ordering
	virtual int rawSlideCount() const {
		return 0;
	}

	virtual QString rawSlideIdentification(int) const = 0;
	virtual QString rawSlideDescription(int) const = 0;

	/// Slide count considering custom ordering
	int slideCount() const;

	inline QString slideIdentification(int i) const {
		return slideOrder_.size() > i ? rawSlideIdentification(slideOrder_[i]) : QString();
	}

	inline QString slideDescription(int i) const {
		return slideOrder_.size() > i ? rawSlideDescription(slideOrder_[i]) : tr("## SNÍMEK NEEXISTUJE ##");
	}

	inline Playlist *playlist() const {
		return playlist_;
	}

	/// Offset of the slides in the playlist (globalSlideId of the first slide of this presentation)
	inline int globalSlideIdOffset() const {
		return globalSlideIdOffset_;
	}

public:
	virtual QString identification() const = 0;
	virtual QPixmap icon() const = 0;

public:
	virtual PresentationEngine *engine() const = 0;

	virtual bool activatePresentation() = 0;
	virtual void deactivatePresentation() = 0;
	virtual bool setSlide(int localSlideId) = 0;

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
