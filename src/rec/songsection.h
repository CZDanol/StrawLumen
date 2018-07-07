#ifndef SONGSECTION_H
#define SONGSECTION_H

#include <QString>
#include <QCoreApplication>
#include <QPixmap>
#include <QVector>
#include <QPair>

class SongSection
{
	Q_DECLARE_TR_FUNCTIONS(SongSection)

public:
	static QVector<SongSection> songSections(const QString &song);

	SongSection();
	explicit SongSection(const QString &str);
	static SongSection customSection(const QString &str);

public:
	bool isValid() const;

	QString standardName() const;
	QString userFriendlyName() const;
	QString annotation() const;

	QPixmap icon() const;

private:
	bool isValid_;
	bool isStandard_;
	QString name_, index_;

};

#endif // SONGSECTION_H
