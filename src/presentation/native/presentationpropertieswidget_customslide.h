#ifndef PRESENTATIONPROPERTIESWIDGET_CUSTOMSLIDE_H
#define PRESENTATIONPROPERTIESWIDGET_CUSTOMSLIDE_H

#include <QWidget>
#include <QSharedPointer>
#include <QTimer>

#include "rec/presentationbackground.h"

namespace Ui {
	class PresentationPropertiesWidget_CustomSlide;
}

class Presentation_CustomSlide;

class PresentationPropertiesWidget_CustomSlide : public QWidget
{
	Q_OBJECT

public:
	explicit PresentationPropertiesWidget_CustomSlide(const QSharedPointer<Presentation_CustomSlide> &presentation, QWidget *parent = 0);
	~PresentationPropertiesWidget_CustomSlide();

private slots:
	void fillData();

private slots:
	void onUpdateTimerTimeout();

private slots:
	void on_wgtStyle_sigPresentationStyleChangedByUser();
	void on_wgtBackground_sigPresentationBackgroundChangedByUser(const PresentationBackground &);
	void on_lnTitle_editingFinished();
	void on_teText_textChanged();

private:
	Ui::PresentationPropertiesWidget_CustomSlide *ui;
	QSharedPointer<Presentation_CustomSlide> presentation_;
	QTimer textUpdateTimer_;

};

#endif // PRESENTATIONPROPERTIESWIDGET_CUSTOMSLIDE_H