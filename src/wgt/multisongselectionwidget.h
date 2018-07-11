#ifndef MULTISONGSELECTIONWIDGET_H
#define MULTISONGSELECTIONWIDGET_H

#include <QWidget>

namespace Ui {
	class MultiSongSelectionWidget;
}

class MultiSongSelectionWidget : public QWidget
{
	Q_OBJECT

public:
	explicit MultiSongSelectionWidget(QWidget *parent = 0);
	~MultiSongSelectionWidget();

private:
	Ui::MultiSongSelectionWidget *ui;
};

#endif // MULTISONGSELECTIONWIDGET_H
