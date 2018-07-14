#include "backgroundmanager.h"

#include <QCoreApplication>
#include <QtConcurrent/QtConcurrent>
#include <QSharedPointer>

#include "util/execonmainthread.h"
#include "gui/backgrounddialog.h"
#include "main.h"

BackgroundManager *backgroundManager = nullptr;

BackgroundManager::BackgroundManager()
{
	userBackgroundDirectory_ = QDir(appDataDirectory.absoluteFilePath("backgrounds"));
	if(!userBackgroundDirectory_.mkpath("."))
		criticalBootError(tr("Nepodařilo se vytvořit složku pro pozadí"));

	internalBackgroundDirectory_ = QDir(QDir(qApp->applicationDirPath()).absoluteFilePath("../backgrounds"));

	cache_.setMaxCost(10);
}

const QDir &BackgroundManager::internalBackgroundDirectory() const
{
	return internalBackgroundDirectory_;
}

const QDir &BackgroundManager::userBackgroundDirectory() const
{
	return userBackgroundDirectory_;
}

const QImage BackgroundManager::getBackground(const QString &filename, const QSize &requiredSize)
{
	Q_UNUSED(requiredSize);

	// Background already in db -> return it
	const QString filePath = (filename.startsWith('_') ? internalBackgroundDirectory_ : userBackgroundDirectory_).absoluteFilePath(filename);

	QImage *img = cache_[filePath];
	if(img)
		return *img;

	// Insert dummy image to the db so we know it's loading
	cache_.insert(filePath, new QImage());

	QPointer<BackgroundManager> thisPtr(this);
	QtConcurrent::run([this, thisPtr, filePath, filename] {
		QImage img(filePath);

		execOnMainThread([this, thisPtr, filePath, img, filename] {
			if(thisPtr.isNull())
				return;

			cache_.insert(filePath, new QImage(img));
			emit sigBackgroundLoaded(filename);
		});
	});

	return QImage();
}

void BackgroundManager::preloadBackground(const QString &filename, const QSize &requiredSize)
{
	getBackground(filename, requiredSize);
}
