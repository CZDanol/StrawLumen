#ifndef PRESENTATION_SONG_H
#define PRESENTATION_SONG_H

#include <QVector>
#include <QMap>
#include <QSharedPointer>
#include <QSqlRecord>

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
		QPixmap icon_;
	};

public:
	static QSharedPointer<Presentation_Song> createFromDb(qlonglong songId);
	static QSharedPointer<Presentation_Song> createFromJSON(const QJsonObject &json);

	QJsonObject toJSON() const override;

public:
	void drawSlide(QPainter &p, int slideId, const QRect &rect) override;

public:
	QString identification() const override;
	QPixmap icon() const override;

	QWidget *createPropertiesWidget(QWidget *parent) override;

	int slideCount() const override;
	QString slideIdentification(int i) const override;
	QString slideDescription(int i) const override;
	QPixmap slideIdentificationIcon(int) const override;

public:
	QString classIdentifier() const override;

private:
	Presentation_Song();

private:
	bool loadFromDb(qlonglong songId);
	bool loadFromDb_Uid(const QString &uid);
	void loadFromDb(const QSqlRecord &rec);

	void loadSlideOrder();

private slots:
	void onDbManagerSongChanged(qlonglong songId);
	void onStyleChanged();
	void onStyleBackgroundChanged();
	void onStyleNeedsRepaint();

private:
	qlonglong songId_ = -1;
	QString songUid_;
	QString name_, author_, defaultSlideOrder_, customSlideOrder_;
	bool emptySlideBefore_ = false, emptySlideAfter_ = false, ignoreEmptySlides_ = false;
	bool wordWrap_ = false;

private:
	PresentationStyle style_;

private:
	QMap<QString, SectionRec> sections_;
	QVector<SlideRec> slides_;

};

#endif // PRESENTATION_SONG_H
