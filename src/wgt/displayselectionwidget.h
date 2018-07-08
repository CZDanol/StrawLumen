#ifndef DISPLAYSELECTIONWIDGET_H
#define DISPLAYSELECTIONWIDGET_H

#include <QWidget>
#include <QScreen>
#include <QPair>

namespace Ui {
	class DisplaySelectionWidget;
}

class DisplaySelectionWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(bool preferNonprimaryScreens MEMBER isPreferNonprimaryScreens_)

public:
	explicit DisplaySelectionWidget(QWidget *parent = 0);
	~DisplaySelectionWidget();

signals:
	void sigCurrentChangedByUser(QScreen* current);

public:
	QScreen *selectedScreen() const;
	QPair<QRect,QString> selectedScreenId() const;
	void setSelectedScreen(const QPair<QRect,QString> &id);

public slots:
	void updateScreenList();

private slots:
	void onItemActivated(int current);

private:
	Ui::DisplaySelectionWidget *ui;
	QList<QScreen*> screenList_;
	QScreen *primaryScreen_;
	int primaryScreenIndex_ = 0, anyNonprimaryScreenIndex_ = 0;
	bool isPreferNonprimaryScreens_ = false;

};

#endif // DISPLAYSELECTIONWIDGET_H
