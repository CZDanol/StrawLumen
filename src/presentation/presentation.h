#ifndef PRESENTATION_H
#define PRESENTATION_H

#include <QObject>
#include <QSharedPointer>
#include <QPixmap>

class Playlist;

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

	virtual QString rawSlideIdentification(int) const {
		return QString();
	}

	virtual QString rawSlideDescription(int) const {
		return QString();
	}

	/// Slide count considering custom ordering
	int slideCount() const;

	inline QString slideIdentification(int i) const {
		return slideOrder_.size() > i ? rawSlideIdentification(slideOrder_[i]) : QString();
	}

	inline QString slideDescription(int i) const {
		return slideOrder_.size() > i ? rawSlideDescription(slideOrder_[i]) : tr("## SNÍMEK NEEXISTUJE ##");
	}

	inline int firstSlideOffsetInPlaylist() const {
		return firstSlideOffsetInPlaylist_;
	}

public:
	virtual QString identification() const {
		return QString();
	}
	virtual QPixmap icon() const {
		return QPixmap();
	}

protected:
	Presentation() {}

	void initDefaultSlideOrder();

protected:
	QVector<int> slideOrder_;

private:
	Playlist *playlist_ = nullptr;
	int positionInPlaylist_ = -1;
	int firstSlideOffsetInPlaylist_ = -1;

};

Q_DECLARE_METATYPE(QSharedPointer<Presentation>)

#endif // PRESENTATION_H
