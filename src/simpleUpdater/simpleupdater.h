#ifndef SIMPLEUPDATER_H
#define SIMPLEUPDATER_H

#include <QDialog>

namespace Ui {
	class SimpleUpdater;
}

class SimpleUpdater : public QDialog
{
	Q_OBJECT

public:
	explicit SimpleUpdater(QWidget *parent = 0);
	~SimpleUpdater();

private:
	Ui::SimpleUpdater *ui;

};

SimpleUpdater *simpleUpdater();

#endif // SIMPLEUPDATER_H
