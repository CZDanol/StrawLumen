#ifndef DISPLAYSELECTIONWIDGET_H
#define DISPLAYSELECTIONWIDGET_H

#include <QWidget>
#include <QScreen>

namespace Ui {
	class DisplaySelectionWidget;
}

class DisplaySelectionWidget : public QWidget
{
	Q_OBJECT

public:
	explicit DisplaySelectionWidget(QWidget *parent = 0);
	~DisplaySelectionWidget();

signals:
	void sigCurrentChanged(QScreen* current);

public:
	QScreen *selectedScreen() const;

public slots:
	void updateScreenList();

private slots:
	void onCurrentIndexChanged(int current);

private:
	Ui::DisplaySelectionWidget *ui;
	QList<QScreen*> screenList_;
	QScreen *primaryScreen_;

};

#endif // DISPLAYSELECTIONWIDGET_H
