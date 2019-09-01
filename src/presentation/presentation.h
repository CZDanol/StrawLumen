#ifndef PRESENTATION_H
#define PRESENTATION_H

#include <QObject>
#include <QSharedPointer>
#include <QPixmap>
#include <QJsonObject>

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
	/// Is emitted when slide order/description/anything has changed for this presentation
	void sigSlidesChanged();

	/// Is emitted when the presentation icon/title has changed (anything that is visible from the playlist view)
	void sigItemChanged(Presentation *presentation);

	/// Is emitted whenever ANYTHING about the presentation is changed (for saving tracking purposes)
	void sigChanged();

	void sigMorphedInto(const QSharedPointer<Presentation> &from, const QSharedPointer<Presentation> &to);

public:
	virtual int slideCount() const = 0;

	virtual QString slideIdentification(int slideId) const;
	virtual QPixmap slideIdentificationIcon(int slideId) const;
	virtual QString slideDescription(int slideId) const;

	Playlist *playlist() const;
	int positionInPlaylist() const;

	/// Offset of the slides in the playlist (globalSlideId of the first slide of this presentation)
	int globalSlideIdOffset() const;

public:
	virtual QString identification() const = 0;
	virtual QPixmap icon() const = 0;
	virtual QPixmap specialIcon() const;

	virtual QWidget *createPropertiesWidget(QWidget *parent);

public:
	virtual PresentationEngine *engine() const = 0;
	virtual QString classIdentifier() const = 0;

public:
	virtual QJsonObject toJSON() const = 0;

public:
	/// DO NOT CALL, ONLY TO BE CALLED BY PRESENTATIONMANAGER
	virtual void activatePresentation(int startingSlide) = 0;
	/// DO NOT CALL, ONLY TO BE CALLED BY PRESENTATIONMANAGER
	virtual void deactivatePresentation();
	/// DO NOT CALL, ONLY TO BE CALLED BY PRESENTATIONMANAGER
	virtual void setSlide(int localSlideId, bool force = false);

protected:
	Presentation();

private:
	Playlist *playlist_ = nullptr;
	int positionInPlaylist_ = -1;
	int globalSlideIdOffset_ = -1;

};

Q_DECLARE_METATYPE(QSharedPointer<Presentation>)

#endif // PRESENTATION_H
