#ifndef PRESENTATIONPROPERTIESWIDGET_SONG_H
#define PRESENTATIONPROPERTIESWIDGET_SONG_H

#include <QWidget>
#include <QRegularExpressionValidator>
#include <QCompleter>
#include <QStringListModel>
#include <QMenu>

#include "rec/presentationbackground.h"

namespace Ui {
	class PresentationPropertiesWidget_Song;
}

class Presentation_Song;

class PresentationPropertiesWidget_Song : public QWidget
{
	Q_OBJECT

public:
	explicit PresentationPropertiesWidget_Song(const QSharedPointer<Presentation_Song> &presentation, QWidget *parent = 0);
	~PresentationPropertiesWidget_Song();

private slots:
	void fillData();

private slots:
	void onStyleChangedByUser();
	void on_wgtBackground_sigPresentationBackgroundChangedByUser(const PresentationBackground &);
	void on_lnSlideOrder_editingFinished();
	void on_lnSlideOrder_sigFocused();
	void on_btnAddSlideOrderItem_pressed();
	void on_cbEmptySlideBefore_clicked(bool checked);
	void on_cbEmptySlideAfter_clicked(bool checked);

private:
	Ui::PresentationPropertiesWidget_Song *ui;
	QSharedPointer<Presentation_Song> presentation_;

private:
	QRegularExpressionValidator slideOrderValidator_;
	QCompleter slideOrderCompleter_;
	QStringListModel slideOrderCompleterModel_;
	QMenu *addCustomSlideOrderItemMenu_;

};

#endif // PRESENTATIONPROPERTIESWIDGET_SONG_H
