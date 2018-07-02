#include <QApplication>
#include <QStyleFactory>
#include <QFile>

#include "gui/mainwindow.h"
#include "gui/splashscreen.h"
#include "job/activexjobthread.h"
#include "presentation/presentation.h"
#include "presentation/presentationengine_powerpoint.h"
#include "presentation/presentationmanager.h"

void initApplication();
void uninitApplication();

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	initApplication();

	// Queued so the window shows after the application is fully loaded
	QMetaObject::invokeMethod(mainWindow, "show", Qt::QueuedConnection);

	int result = app.exec();

	uninitApplication();

	return result;
}

void initApplication() {
	qRegisterMetaType<QSharedPointer<Presentation>>();

	// Setup stylesheet
	{
		qApp->setStyle(QStyleFactory::create("Fusion"));

		QPalette p = qApp->palette();
		p.setColor(QPalette::Light, QColor("#ddd") );
		qApp->setPalette(p);

		QFile f( ":/stylesheet.css" );
		f.open( QFile::ReadOnly );
		qApp->setStyleSheet( QString( f.readAll() ) );
	}

	activeXJobThread = new ActiveXJobThread();
	presentationEngine_PowerPoint = new PresentationEngine_PowerPoint();
	presentationManager = new PresentationManager();

	mainWindow = new MainWindow(nullptr);
	splashscreen = new Splashscreen(mainWindow);
}

void uninitApplication() {
	delete mainWindow;

	delete presentationManager;
	delete presentationEngine_PowerPoint;
	delete activeXJobThread;
}
