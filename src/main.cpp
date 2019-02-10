#include "main.h"

#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QStandardPaths>
#include <QLibraryInfo>
#include <QTranslator>

#include "gui/mainwindow.h"
#include "gui/splashscreen.h"
#include "gui/settingsdialog.h"
#include "gui/backgrounddialog.h"
#include "gui/stylesdialog.h"
#include "gui/startupsplashscreen.h"
#include "strawapi/simpleupdater.h"
#include "importexport/lumenexportdialog.h"
#include "job/activexjobthread.h"
#include "job/db.h"
#include "job/settings.h"
#include "job/backgroundmanager.h"
#include "job/asynccachemanager.h"
#include "job/parsebible.h"
#include "presentation/presentation.h"
#include "presentation/powerpoint/presentationengine_powerpoint.h"
#include "presentation/native/presentationengine_native.h"
#include "presentation/native/nativeprojectorwindow.h"
#include "presentation/video/presentationengine_video.h"
#include "presentation/video/videoprojectorwindow.h"
#include "presentation/presentationmanager.h"
#include "util/standarddialogs.h"
#include "util/execonmainthread.h"
#include "util/fusionproxystyle.h"

QDir appDataDirectory;

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	app.setApplicationName("Straw Lumen");
	app.setOrganizationName("Straw Solutions");
	app.setOrganizationDomain("straw-solutions.cz");
	app.setApplicationVersion(PROGRAM_VERSION);

	app.setAttribute(Qt::AA_DisableWindowContextHelpButton);

	setupStylesheet();

	StartupSplashscreen startupSplashscreen;
	startupSplashscreen.show();

	execOnMainThread([&]{
		// Force startupSplashscreen to repaint
		app.processEvents();

		initApplication();
		mainWindow->show();
		simpleUpdater()->checkForUpdates();
		startupSplashscreen.close();

		checkBibleImport();
	});

	int result = app.exec();

	uninitApplication();

	return result;
}

void initApplication() {
	qRegisterMetaType<QSharedPointer<Presentation>>();

	if(QFile::exists(QDir(qApp->applicationDirPath()).absoluteFilePath("../portableMode")))
		appDataDirectory = QDir(QDir(qApp->applicationDirPath()).absoluteFilePath("../programData"));
	else
		appDataDirectory = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

	if( !appDataDirectory.mkpath(".") )
		criticalBootError(DBManager::tr("Nepodařilo se vytvořit složku pro data aplikace: \"%1\"").arg(appDataDirectory.absolutePath()));

	settings = new SettingsManager();
	db = new DatabaseManager();
	backgroundManager = new BackgroundManager();
	asyncCache = new AsyncCacheManager();

	activeXJobThread = new ActiveXJobThread();
	presentationEngine_PowerPoint = new PresentationEngine_PowerPoint();
	presentationEngine_Native = new PresentationEngine_Native();
	presentationEngine_Video = new PresentationEngine_Video();
	presentationManager = new PresentationManager();

	qApp->processEvents();

	mainWindow = new MainWindow();
	nativeProjectorWindow = new NativeProjectorWindow();
	videoProjectorWindow = new VideoProjectorWindow();

	qApp->processEvents();

	settingsDialog = new SettingsDialog(mainWindow);
	splashscreen = new Splashscreen(mainWindow);
	backgroundDialog = new BackgroundDialog(mainWindow);
	stylesDialog = new StylesDialog(mainWindow);
}

void uninitApplication() {
	presentationManager->setActive(false);

	delete mainWindow;
	delete nativeProjectorWindow;
	delete videoProjectorWindow;

	delete presentationManager;
	delete presentationEngine_Native;
	delete presentationEngine_PowerPoint;
	delete presentationEngine_Video;
	delete activeXJobThread;

	delete asyncCache;
	delete backgroundManager;
	delete db;
	delete settings;
}

void setupStylesheet() {
	qApp->setStyle(new FusionProxyStyle());

	QPalette p = qApp->palette();
	p.setColor(QPalette::Light, QColor("#ddd") );
	qApp->setPalette(p);

	QFile f( ":/stylesheet.css" );
	f.open( QFile::ReadOnly );
	qApp->setStyleSheet( QString( f.readAll() ) );
}

void criticalBootError(const QString &message) {
	standardErrorDialog(message, nullptr, true);
	exit(-1);
}
