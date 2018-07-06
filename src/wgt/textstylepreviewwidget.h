#ifndef TEXTSTYLEPREVIEWWIDGET_H
#define TEXTSTYLEPREVIEWWIDGET_H

#include <QFrame>
#include <QPainterPath>

#include "util/textstyle.h"

class TextStylePreviewWidget : public QFrame
{
	Q_OBJECT

public:
	explicit TextStylePreviewWidget(QWidget *parent = nullptr);

public slots:
	void setTextStyle(const TextStyle &style);
	void forceSetTextStyle(const TextStyle &style);

protected:
	void paintEvent(QPaintEvent *);

private:
	TextStyle textStyle_;
	QString previewText_;
	bool wasFirstUpdate_ = false;

private:
	QRect previewTextRect_;
	QPainterPath textPath_;

};

#endif // TEXTSTYLEPREVIEWWIDGET_H
