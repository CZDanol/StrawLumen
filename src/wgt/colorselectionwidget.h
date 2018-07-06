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

signals:
	void sigColorChanged(QColor newColor);

public slots:
	void setColor(const QColor &color);

private:
	Ui::ColorSelectionWidget *ui;
	QColor currentColor_;

};

#endif // COLORSELECTWIDGET_H
