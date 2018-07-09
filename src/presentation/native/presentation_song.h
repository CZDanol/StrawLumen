#ifndef PRESENTATION_SONG_H
#define PRESENTATION_SONG_H

#include <QVector>
#include <QHash>
#include <QSharedPointer>

#include "presentation/native/presentation_nativepresentation.h"
#include "rec/songsection.h"
#include "rec/presentationstyle.h"

class Presentation_Song : public Presentation_NativePresentation
{
	Q_OBJECT

	friend class PresentationPropertiesWidget_Song;

	struct SlideData {
		QString content_;
		QString description_;
	};
	struct SectionRec {
		SongSection section;
		QVector<SlideData> slides;
	};
	struct SlideRec {
		QString name_;
		QString content_;
		QString description_;
	};

public:
	static QSharedPointer<Presentation_Song> createFromDb(qlonglong songId);

public:
	void drawSlide(QPainter &p, int slideId, const QRect &rect) override;

public:
	QString identification() const override;
	QPixmap icon() const override;

	QWidget *createPropertiesWidget(QWidget *parent) override;

	int slideCount() const override;
	QString slideIdentification(int i) const override;
	QString slideDescription(int i) const override;

private:
	Presentation_Song();

private:
	void loadFromDb(qlonglong songId);

	void loadSlideOrder();

private slots:
	void onDbManagerSongChanged(qlonglong songId);
	void onStyleChanged();
	void onStyleBackgroundChanged();

private:
	qlonglong songId_ = -1;
	QString name_, author_, defaultSlideOrder_, customSlideOrder_;
	PresentationStyle style_;
	QHash<QString, SectionRec> sections_;
	QVector<SlideRec> slides_;

};

#endif // PRESENTATION_SONG_H
