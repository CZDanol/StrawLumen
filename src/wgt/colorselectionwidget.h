#ifndef COLORSELECTWIDGET_H
#define COLORSELECTWIDGET_H

#include <QWidget>

namespace Ui {
	class ColorSelectionWidget;
}

class ColorSelectionWidget : public QWidget {
	Q_OBJECT
	Q_PROPERTY(bool alphaChannelEnabled READ isAlphaChannelEnabled() WRITE setAlphaChannelEnabled())

public:
	explicit ColorSelectionWidget(QWidget *parent = 0);
	~ColorSelectionWidget();

signals:
	void sigColorChangedByUser(QColor newColor);

public slots:
	void setColor(const QColor &color);
	void setAlphaChannelEnabled(bool set);
	void setReadOnly(bool set);

public:
	bool isAlphaChannelEnabled() const;

private slots:
	void onComponentColorChangedByUser(const QColor &newColor);

private slots:
	void on_lnHex_editingFinished();

private:
	Ui::ColorSelectionWidget *ui;
	QColor currentColor_;
	bool isReadOnly_ = false;
	bool isAlphaChannelEnabled_ = true;
};

#endif// COLORSELECTWIDGET_H
