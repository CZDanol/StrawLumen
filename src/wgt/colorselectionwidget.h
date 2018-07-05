#ifndef COLORSELECTWIDGET_H
#define COLORSELECTWIDGET_H

#include <QWidget>

namespace Ui {
	class ColorSelectionWidget;
}

class ColorSelectionWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ColorSelectionWidget(QWidget *parent = 0);
	~ColorSelectionWidget();

private slots:
	void on_btnSelect_clicked();

private:
	Ui::ColorSelectionWidget *ui;

};

#endif // COLORSELECTWIDGET_H
