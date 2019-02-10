#ifndef PRESENTATIONPROPERTIESWIDGET_BIBLEVERSE_H
#define PRESENTATIONPROPERTIESWIDGET_BIBLEVERSE_H

#include <QWidget>

#include "rec/presentationbackground.h"

namespace Ui {
	class PresentationPropertiesWidget_BibleVerse;
}

class Presentation_BibleVerse;

class PresentationPropertiesWidget_BibleVerse : public QWidget
{
	Q_OBJECT

public:
	explicit PresentationPropertiesWidget_BibleVerse(const QSharedPointer<Presentation_BibleVerse> &presentation, QWidget *parent = nullptr);
	~PresentationPropertiesWidget_BibleVerse();

private slots:
	void fillData();

private slots:
	void on_wgtStyle_sigPresentationStyleChangedByUser();
	void on_wgtBackground_sigPresentationBackgroundChangedByUser(const PresentationBackground &);

	void on_btnWizard_clicked();

private:
	Ui::PresentationPropertiesWidget_BibleVerse *ui;
	QSharedPointer<Presentation_BibleVerse> presentation_;

};

#endif // PRESENTATIONPROPERTIESWIDGET_BIBLEVERSE_H
