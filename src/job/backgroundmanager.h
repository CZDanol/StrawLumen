#ifndef BACKGROUNDMANAGER_H
#define BACKGROUNDMANAGER_H

#include <QObject>
#include <QHash>
#include <QImage>
#include <QDir>

class BackgroundManager : public QObject
{
	Q_OBJECT

public:
	BackgroundManager();

signals:
	void sigBackgroundLoaded(QString filename);

public:
	const QDir &internalBackgroundDirectory() const;
	const QDir &userBackgroundDirectory() const;

public:
	const QImage &background(const QString &filename, const QSize &requiredSize);

private:
	QHash<QString,QImage> db_;
	QDir internalBackgroundDirectory_, userBackgroundDirectory_;

};

extern BackgroundManager *backgroundManager;

#endif // BACKGROUNDMANAGER_H
