#include "presentation_bibleverse_wizard.h"
#include "ui_presentation_bibleverse_wizard.h"

#include "gui/mainwindow.h"

Presentation_BibleVerse_Wizard::Presentation_BibleVerse_Wizard(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Presentation_BibleVerse_Wizard)
{
	ui->setupUi(this);

	connect(ui->wgtVerseSelection, &BibleVerseSelectionWidget::sigSelectionChanged, this, &Presentation_BibleVerse_Wizard::onSelectionChanged);
}

Presentation_BibleVerse_Wizard::~Presentation_BibleVerse_Wizard()
{
	delete ui;
}

void Presentation_BibleVerse_Wizard::exec(const Presentation_BibleVerse_Wizard::AddCallback &addCallback)
{
	addCallback_ = addCallback;
	QDialog::exec();
	addCallback_ = nullptr;
}

void Presentation_BibleVerse_Wizard::onSelectionChanged()
{
	ui->btnInsert->setEnabled(ui->wgtVerseSelection->bibleRef().isValid());
}

Presentation_BibleVerse_Wizard *presentation_BibleVerse_wizard()
{
	static Presentation_BibleVerse_Wizard *result = new Presentation_BibleVerse_Wizard(mainWindow);
	return result;
}

void Presentation_BibleVerse_Wizard::on_btnDone_clicked()
{
	accept();
}

void Presentation_BibleVerse_Wizard::on_btnInsert_clicked()
{
	addCallback_(ui->wgtVerseSelection->bibleRef());
}
