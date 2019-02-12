#ifndef PRESENTATION_BIBLEVERSE_WIZARD_H
#define PRESENTATION_BIBLEVERSE_WIZARD_H

#include <functional>

#include <QDialog>

#include "rec/bibleref.h"

namespace Ui {
	class Presentation_BibleVerse_Wizard;
}

class Presentation_BibleVerse_Wizard : public QDialog
{
	Q_OBJECT

public:
	using AddCallback = std::function<void(const BibleRef&)>;

public:
	explicit Presentation_BibleVerse_Wizard(QWidget *parent = nullptr);
	~Presentation_BibleVerse_Wizard();

public:
	void exec(const AddCallback &addCallback);

private slots:
	void onSelectionChanged();

private slots:
	void on_btnDone_clicked();
	void on_btnInsert_clicked();

private:
	Ui::Presentation_BibleVerse_Wizard *ui;
	AddCallback addCallback_;

};

Presentation_BibleVerse_Wizard *presentation_BibleVerse_wizard();

#endif // PRESENTATION_BIBLEVERSE_WIZARD_H
