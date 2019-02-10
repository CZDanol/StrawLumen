#ifndef PRESENTATION_BIBLEVERSE_WIZARD_H
#define PRESENTATION_BIBLEVERSE_WIZARD_H

#include <QDialog>

namespace Ui {
	class Presentation_BibleVerse_Wizard;
}

class Presentation_BibleVerse_Wizard : public QDialog
{
	Q_OBJECT

public:
	explicit Presentation_BibleVerse_Wizard(QWidget *parent = nullptr);
	~Presentation_BibleVerse_Wizard();

private:
	Ui::Presentation_BibleVerse_Wizard *ui;

};

Presentation_BibleVerse_Wizard *presentation_BibleVerse_wizard();

#endif // PRESENTATION_BIBLEVERSE_WIZARD_H
