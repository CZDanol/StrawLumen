#ifndef PROJECTORWINDOW_H
#define PROJECTORWINDOW_H

#include <QMainWindow>

namespace Ui {
	class ProjectorWindow;
}

class ProjectorWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ProjectorWindow(QWidget *parent = 0);
	~ProjectorWindow();

protected:
	void paintEvent(QPaintEvent *) override;

private:
	Ui::ProjectorWindow *ui;

};

#endif // PROJECTORWINDOW_H
