#include "presentation_bibleverse_wizard.h"
#include "ui_presentation_bibleverse_wizard.h"

#include "gui/mainwindow.h"

Presentation_BibleVerse_Wizard::Presentation_BibleVerse_Wizard(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Presentation_BibleVerse_Wizard)
{
	ui->setupUi(this);
}

Presentation_BibleVerse_Wizard::~Presentation_BibleVerse_Wizard()
{
	delete ui;
}

Presentation_BibleVerse_Wizard *presentation_BibleVerse_wizard()
{
	static Presentation_BibleVerse_Wizard *result = new Presentation_BibleVerse_Wizard(mainWindow);
	return result;
}
