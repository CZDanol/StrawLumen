#include "backgroundmanager.h"

#include <QCoreApplication>
#include <QtConcurrent/QtConcurrent>

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
}

const QDir &BackgroundManager::internalBackgroundDirectory() const
{
	return internalBackgroundDirectory_;
}

const QDir &BackgroundManager::userBackgroundDirectory() const
{
	return userBackgroundDirectory_;
}

const QImage &BackgroundManager::background(const QString &filename, const QSize &requiredSize)
{
	// Background already in db -> return it
	const QString filePath = (filename.startsWith('_') ? internalBackgroundDirectory_ : userBackgroundDirectory_).absoluteFilePath(filename);

	if(db_.contains(filePath))
		return db_[filePath];

	// Insert dummy image to the db so we know it's loading
	db_.insert(filePath, QImage());

	QPointer<BackgroundManager> thisPtr(this);
	QtConcurrent::run([this, thisPtr, filePath, filename] {
		QImage img(filePath);

		execOnMainThread([this, thisPtr, filePath, img, filename] {
			if(thisPtr.isNull())
				return;

			db_[filePath] = img;
			emit sigBackgroundLoaded(filename);
		});
	});

	return QImage();
}
