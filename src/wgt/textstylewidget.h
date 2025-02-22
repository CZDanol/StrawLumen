#ifndef TEXTSTYLEWIDGET_H
#define TEXTSTYLEWIDGET_H

#include <QWidget>

#include "rec/textstyle.h"

namespace Ui {
	class TextStyleWidget;
}

class TextStyleWidget : public QWidget {
	Q_OBJECT

public:
	explicit TextStyleWidget(QWidget *parent = 0);
	~TextStyleWidget();

signals:
	void sigTextStyleChangedByUser(const TextStyle &newStyle);

public:
	const TextStyle &textStyle() const;

public slots:
	void setTextStyle(const TextStyle &style);
	void forceSetTextStyle(const TextStyle &style);
	void setReadOnly(bool set);

private slots:
	void on_wgtFont_sigFontChangedByUser(const QFont &);
	void on_wgtColor_sigColorChangedByUser(const QColor &);
	void on_cbOutline_clicked(bool checked);
	void on_sbOutlineWidth_valueChanged(int arg1);
	void on_wgtOutlineColor_sigColorChangedByUser(const QColor &);
	void on_cbBackground_clicked(bool checked);
	void on_sbBackgroundPadding_valueChanged(int arg1);
	void on_wgtBackgroundColor_sigColorChangedByUser(const QColor &);

private:
	Ui::TextStyleWidget *ui;
	TextStyle textStyle_;
	bool isReadOnly_ = false;
	bool isSettingUp_ = false;
};

#endif// TEXTSTYLEWIDGET_H
