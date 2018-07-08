#include "fontdatabase.h"

#include <QApplication>

QFontDatabase &fontDatabase()
{
	static QFontDatabase *db = nullptr;

	if(!db) {
		db = new QFontDatabase();
		QObject::connect(qApp, &QCoreApplication::aboutToQuit, [=](){
			delete db;
		});
	}

	return *db;
}
