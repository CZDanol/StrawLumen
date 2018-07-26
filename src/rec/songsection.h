#ifndef SONGSECTION_H
#define SONGSECTION_H

#include <QString>
#include <QCoreApplication>
#include <QPixmap>
#include <QVector>
#include <QRegularExpression>

class SongSection
{
	Q_DECLARE_TR_FUNCTIONS(SongSection)

public:
	SongSection();
	explicit SongSection(const QString &str);
	static SongSection customSection(const QString &str);

public:
	bool isValid() const;

	QString standardName() const;
	QString userFriendlyName() const;
	QString shorthandName() const;
	QString annotation() const;

	QPixmap icon() const;

private:
	bool isValid_;
	bool isStandard_;
	QString name_, index_;

};

struct SongSectionWithContent {
	SongSection section;
	QString content;
	int contentPos;
	int annotationPos;
	int untrimmedContentEnd;
};

/// Captures: 0 - whole annotation; 1 - annotation prefix, 2 - section name, 3 - annotation suffix
const QRegularExpression &songSectionAnnotationRegex();

const QRegularExpression &songSlideSeparatorRegex();
const QRegularExpression &songCustomSlideOrderRegex();

QVector<SongSection> songSections(const QString &song);
QVector<SongSectionWithContent> songSectionsWithContent(const QString &song);

#endif // SONGSECTION_H
