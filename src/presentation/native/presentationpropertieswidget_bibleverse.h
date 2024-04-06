#ifndef PRESENTATIONPROPERTIESWIDGET_BIBLEVERSE_H
#define PRESENTATIONPROPERTIESWIDGET_BIBLEVERSE_H

#include <QTimer>
#include <QWidget>

#include "rec/presentationbackground.h"

namespace Ui {
	class PresentationPropertiesWidget_BibleVerse;
}

class Presentation_BibleVerse;

class PresentationPropertiesWidget_BibleVerse : public QWidget {
	Q_OBJECT

public:
	explicit PresentationPropertiesWidget_BibleVerse(const QSharedPointer<Presentation_BibleVerse> &presentation, QWidget *parent = nullptr);
	~PresentationPropertiesWidget_BibleVerse();

private slots:
	void fillData();

private slots:
	void onTextUpdateTimerTimeout();

private slots:
	void on_wgtStyle_sigPresentationStyleChangedByUser();
	void on_wgtBackground_sigPresentationBackgroundChangedByUser(const PresentationBackground &);
	void on_btnWizard_clicked();
	void on_btnMorphIntoCustomSlide_clicked();

	void on_verses_modificationChanged(bool arg1);

private:
	Ui::PresentationPropertiesWidget_BibleVerse *ui;
	QSharedPointer<Presentation_BibleVerse> presentation_;
	QTimer textUpdateTimer_;
	int isSettingUp_ = 0;
};

#endif// PRESENTATIONPROPERTIESWIDGET_BIBLEVERSE_H
