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
bool isPortableMode;

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	app.setApplicationName("Straw Lumen");
	app.setOrganizationName("Straw Solutions");
	app.setOrganizationDomain("straw-solutions.cz");
	app.setApplicationVersion(PROGRAM_VERSION);

	app.setAttribute(Qt::AA_DisableWindowContextHelpButton);

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
#define PROC qApp->processEvents();
	qRegisterMetaType<QSharedPointer<Presentation>>();

	isPortableMode = QFile::exists(QDir(qApp->applicationDirPath()).absoluteFilePath("../portableMode"));

	if(isPortableMode)
		appDataDirectory = QDir(QDir(qApp->applicationDirPath()).absoluteFilePath("../programData"));
	else
		appDataDirectory = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

	if(!appDataDirectory.mkpath(".")) {
		if(appDataDirectory.path().contains("program files", Qt::CaseInsensitive))
			criticalBootError(DBManager::tr("Nepodařilo se vytvořit složku pro data aplikace: \"%1\". To je pravděpodobně tím, že máte nainstalovanou přenosnou verzi aplikace ve složce 'Program Files'. Přenosná verze aplikace musí být nainstalována do umístění, kam uživatel může zapisovat.").arg(appDataDirectory.absolutePath()));
		else
			criticalBootError(DBManager::tr("Nepodařilo se vytvořit složku pro data aplikace: \"%1\"").arg(appDataDirectory.absolutePath()));
	}

	PROC settings = new SettingsManager();
	PROC setupStylesheet(settings->value("darkMode", true).toBool());

	PROC db = new DatabaseManager();
	PROC backgroundManager = new BackgroundManager();
	PROC asyncCache = new AsyncCacheManager();

	PROC activeXJobThread = new ActiveXJobThread();
	PROC presentationEngine_powerPoint = new PresentationEngine_PowerPoint();
	PROC presentationEngine_native = new PresentationEngine_Native();
	PROC presentationEngine_video = new PresentationEngine_Video();
	PROC presentationEngine_web = new PresentationEngine_Web();
	PROC presentationManager = new PresentationManager();

	PROC mainWindow = new MainWindow();
	PROC settingsDialog = new SettingsDialog(mainWindow);

	PROC nativeProjectorWindow = new NativeProjectorWindow();
	PROC videoProjectorWindow = new VideoProjectorWindow();
	PROC webProjectorWindow = new WebProjectorWindow();

	PROC splashscreen = new Splashscreen(mainWindow);
	PROC backgroundDialog = new BackgroundDialog(mainWindow);
	PROC stylesDialog = new StylesDialog(mainWindow);

	PROC QtWebEngine::initialize();

	PROC
#undef PROC
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

void setupStylesheet(bool darkMode) {
	static auto style = new FusionProxyStyle();
	qApp->setStyle(style);

	static auto defaultPalette = qApp->palette();
	QPalette p = defaultPalette;
	if(darkMode) {
		const QColor themeCl("#CA3406");
		const QColor midCl("#333333");
		const QColor darkCl("#151515");
		const QColor veryDarkCl("#080808");

		p.setColor(QPalette::Window, darkCl);
		p.setColor(QPalette::WindowText, Qt::white);
		p.setColor(QPalette::Base, veryDarkCl);
		p.setColor(QPalette::AlternateBase, darkCl);
		p.setColor(QPalette::ToolTipBase, Qt::white);
		p.setColor(QPalette::ToolTipText, Qt::white);
		p.setColor(QPalette::Text, Qt::white);
		p.setColor(QPalette::Button, darkCl);
		p.setColor(QPalette::ButtonText, Qt::white);
		p.setColor(QPalette::BrightText, Qt::red);
		p.setColor(QPalette::Link, QColor("#0a80ff"));
		p.setColor(QPalette::LinkVisited, themeCl);

		p.setColor(QPalette::Highlight, QColor("#f5734c"));
		p.setColor(QPalette::HighlightedText, Qt::black);

		p.setColor(QPalette::Dark, veryDarkCl);
		p.setColor(QPalette::Mid, darkCl);
		p.setColor(QPalette::Midlight, "#222");
		p.setColor(QPalette::Light, midCl);
	}
	else {
		p.setColor(QPalette::Highlight, QColor("#90dff9"));
		p.setColor(QPalette::HighlightedText, Qt::black);

		p.setColor(QPalette::Light, QColor("#ddd"));
	}
	qApp->setPalette(p);

	QFile f(":/stylesheet.css");
	f.open( QFile::ReadOnly );
	qApp->setStyleSheet(QString(f.readAll()));
}

void criticalBootError(const QString &message) {
	standardErrorDialog(message, nullptr, true);
	exit(-1);
}
