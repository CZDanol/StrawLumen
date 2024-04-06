#ifndef STARTUPSPLASHSCREEN_H
#define STARTUPSPLASHSCREEN_H

#include <QDialog>

namespace Ui {
	class StartupSplashscreen;
}

class StartupSplashscreen : public QDialog {
	Q_OBJECT

public:
	explicit StartupSplashscreen(QWidget *parent = 0);
	~StartupSplashscreen();

private:
	Ui::StartupSplashscreen *ui;
};

#endif// STARTUPSPLASHSCREEN_H
