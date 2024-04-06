#ifndef STYLESELECTIONWIDGET_H
#define STYLESELECTIONWIDGET_H

#include <QWidget>

#include "rec/presentationstyle.h"

namespace Ui {
	class StyleSelectionWidget;
}

class StyleSelectionWidget : public QWidget {
	Q_OBJECT

public:
	explicit StyleSelectionWidget(QWidget *parent = 0);
	~StyleSelectionWidget();

signals:
	void sigPresentationStyleChangedByUser();

public:
	const PresentationStyle &presentationStyle() const;

public slots:
	void setPresentationStyle(const PresentationStyle &style);

private slots:
	void updateLabel();

private slots:
	void on_btnSelect_clicked();

private:
	Ui::StyleSelectionWidget *ui;
	PresentationStyle presentationStyle_;
};

#endif// STYLESELECTIONWIDGET_H
