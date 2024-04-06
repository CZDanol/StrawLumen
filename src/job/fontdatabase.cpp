#include "fontdatabase.h"

#include <QApplication>

QFontDatabase &fontDatabase() {
	static QFontDatabase *inst = nullptr;

	if(!inst) {
		inst = new QFontDatabase();
		QObject::connect(qApp, &QCoreApplication::aboutToQuit, [] {
			delete inst;
		});
	}

	return *inst;
}
