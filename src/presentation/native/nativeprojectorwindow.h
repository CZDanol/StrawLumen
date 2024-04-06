#ifndef NATIVEPROJECTORWINDOW_H
#define NATIVEPROJECTORWINDOW_H

#include <QKeyEvent>
#include <QMainWindow>

namespace Ui {
	class NativeProjectorWindow;
}

class NativeProjectorWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit NativeProjectorWindow(QWidget *parent = nullptr);
	~NativeProjectorWindow() override;

protected:
	virtual void paintEvent(QPaintEvent *) override;
	virtual void changeEvent(QEvent *) override;

private:
	Ui::NativeProjectorWindow *ui;
};

extern NativeProjectorWindow *nativeProjectorWindow;

#endif// NATIVEPROJECTORWINDOW_H
