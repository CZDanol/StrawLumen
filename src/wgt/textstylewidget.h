#ifndef TEXTSTYLEWIDGET_H
#define TEXTSTYLEWIDGET_H

#include <QWidget>

#include "util/textstyle.h"

namespace Ui {
	class TextStyleWidget;
}

class TextStyleWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TextStyleWidget(QWidget *parent = 0);
	~TextStyleWidget();

signals:
	void sigTextStyleChanged(const TextStyle &newStyle);

public:
	const TextStyle &textStyle() const;

public slots:
	void setTextStyle(const TextStyle &style);
	void forceSetTextStyle(const TextStyle &style);

private slots:
	void on_wgtFont_sigFontChanged(const QFont &);
	void on_wgtColor_sigColorChanged(const QColor &);
	void on_cbOutline_clicked(bool checked);
	void on_sbOutlineWidth_valueChanged(int arg1);
	void on_wgtOutlineColor_sigColorChanged(const QColor &);
	void on_cbBackground_clicked(bool checked);
	void on_cbBackgroundPadding_valueChanged(int arg1);
	void on_wgtBackgroundColor_sigColorChanged(const QColor &);

private:
	Ui::TextStyleWidget *ui;
	TextStyle textStyle_;
	bool isSettingUp_ = false;

};

#endif // TEXTSTYLEWIDGET_H
