#ifndef WEBPROJECTORWINDOW_H
#define WEBPROJECTORWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include <QWebEngineView>

namespace Ui {
	class WebProjectorWindow;
}

class WebProjectorWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit WebProjectorWindow(QWidget *parent = nullptr);
	~WebProjectorWindow() override;

public:
	void setUrl(const QUrl &url);
	void setBlackScreen(bool set);

protected:
	virtual void changeEvent(QEvent *e) override;

private:
	Ui::WebProjectorWindow *ui;
	QWebEngineView *webView_;

};

extern WebProjectorWindow *webProjectorWindow;

#endif // WEBPROJECTORWINDOW_H
