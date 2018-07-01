#include <QApplication>
#include <QStyleFactory>
#include <QFile>

#include "gui/mainwindow.h"

void initApplication();
void uninitApplication();

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	initApplication();

	mainWindow->show();
	int result = app.exec();

	uninitApplication();

	return result;
}

void initApplication() {
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

	mainWindow = new MainWindow();
}
void uninitApplication() {
	delete mainWindow;
}
