#ifndef PRESENTATIONBACKGROUNDSELECTIONWIDGET_H
#define PRESENTATIONBACKGROUNDSELECTIONWIDGET_H

#include <QWidget>

#include "rec/presentationbackground.h"

namespace Ui {
	class PresentationBackgroundSelectionWidget;
}

class PresentationBackgroundSelectionWidget : public QWidget
{
	Q_OBJECT

public:
	explicit PresentationBackgroundSelectionWidget(QWidget *parent = 0);
	~PresentationBackgroundSelectionWidget();

signals:
	void sigPresentationBackgroundChangedByUser(PresentationBackground newBackground);

public slots:
	void setPresentationBackground(const PresentationBackground &background);
	void setReadOnly(bool set);

private:
	void updateLabel();

private slots:
	void on_btnSelect_clicked();

private:
	Ui::PresentationBackgroundSelectionWidget *ui;
	PresentationBackground presentationBackground_;
	bool isReadOnly_ = false;

};

#endif // PRESENTATIONBACKGROUNDSELECTIONWIDGET_H
