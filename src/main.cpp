#include "main.h"

#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QStandardPaths>
#include <QLibraryInfo>
#include <QTranslator>
#include <QtWebEngine/QtWebEngine>

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
#include "presentation/web/presentationengine_web.h"
#include "presentation/web/webprojectorwindow.h"
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
	qApp->processEvents();

	db = new DatabaseManager();
	qApp->processEvents();

	backgroundManager = new BackgroundManager();
	qApp->processEvents();

	asyncCache = new AsyncCacheManager();
	qApp->processEvents();

	activeXJobThread = new ActiveXJobThread();
	presentationEngine_powerPoint = new PresentationEngine_PowerPoint();
	presentationEngine_native = new PresentationEngine_Native();
	presentationEngine_video = new PresentationEngine_Video();
	presentationEngine_web = new PresentationEngine_Web();
	presentationManager = new PresentationManager();

	qApp->processEvents();

	mainWindow = new MainWindow();
	nativeProjectorWindow = new NativeProjectorWindow();
	videoProjectorWindow = new VideoProjectorWindow();
	webProjectorWindow = new WebProjectorWindow();

	qApp->processEvents();

	settingsDialog = new SettingsDialog(mainWindow);
	splashscreen = new Splashscreen(mainWindow);
	backgroundDialog = new BackgroundDialog(mainWindow);
	stylesDialog = new StylesDialog(mainWindow);

	qApp->processEvents();

	QtWebEngine::initialize();

	qApp->processEvents();
}

void uninitApplication() {
	presentationManager->setActive(false);

	delete webProjectorWindow;
	delete videoProjectorWindow;
	delete nativeProjectorWindow;
	delete mainWindow;

	delete presentationManager;
	delete presentationEngine_native;
	delete presentationEngine_powerPoint;
	delete presentationEngine_video;
	delete presentationEngine_web;
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
