#ifndef BACKGROUNDMANAGER_H
#define BACKGROUNDMANAGER_H

#include <QObject>
#include <QCache>
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
	const QImage getBackground(const QString &filename, const QSize &requiredSize);
	void preloadBackground(const QString &filename, const QSize &requiredSize);

private:
	QCache<QString,QImage> cache_;
	QImage nullImage_;
	QDir internalBackgroundDirectory_, userBackgroundDirectory_;

};

extern BackgroundManager *backgroundManager;

#endif // BACKGROUNDMANAGER_H
