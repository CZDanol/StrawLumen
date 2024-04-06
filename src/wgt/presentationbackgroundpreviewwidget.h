#ifndef PRESENTATIONBACKGROUNDPREVIEWWIDGET_H
#define PRESENTATIONBACKGROUNDPREVIEWWIDGET_H

#include <QWidget>

#include "rec/presentationbackground.h"

class PresentationBackgroundPreviewWidget : public QWidget {
	Q_OBJECT

public:
	explicit PresentationBackgroundPreviewWidget(QWidget *parent = nullptr);

public slots:
	void setPresentationBackground(const PresentationBackground &background);

protected:
	void paintEvent(QPaintEvent *event) override;

private slots:
	void onNeedsRepaint();

private:
	PresentationBackground presentationBackground_;
};

#endif// PRESENTATIONBACKGROUNDPREVIEWWIDGET_H
