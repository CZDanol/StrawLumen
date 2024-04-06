#include "presentation_bibleverse_wizard.h"
#include "ui_presentation_bibleverse_wizard.h"

#include "gui/mainwindow.h"
#include "job/settings.h"

// settingName, uiControl
#define BIBLEVERSE_SETTINGS_FACTORY(F)       \
	F(bibleVerse_autoSplitEnable, cbAutoSplit) \
	F(bibleVerse_autoSplitRange, sbAutoSplit)

Presentation_BibleVerse_Wizard::Presentation_BibleVerse_Wizard(QWidget *parent) : QDialog(parent),
                                                                                  ui(new Ui::Presentation_BibleVerse_Wizard) {
	ui->setupUi(this);

	connect(ui->wgtVerseSelection, &BibleVerseSelectionWidget::sigSelectionChanged, this, &Presentation_BibleVerse_Wizard::onSelectionChanged);

	BIBLEVERSE_SETTINGS_FACTORY(SETTINGS_LINK)
}

Presentation_BibleVerse_Wizard::~Presentation_BibleVerse_Wizard() {
	delete ui;
}

void Presentation_BibleVerse_Wizard::exec(const Presentation_BibleVerse_Wizard::AddCallback &addCallback) {
	addCallback_ = addCallback;
	QDialog::exec();
	addCallback_ = nullptr;
}

void Presentation_BibleVerse_Wizard::onSelectionChanged() {
	ui->btnInsert->setEnabled(ui->wgtVerseSelection->bibleRef().isValid());
}

Presentation_BibleVerse_Wizard *presentation_BibleVerse_wizard() {
	static Presentation_BibleVerse_Wizard *result = new Presentation_BibleVerse_Wizard(mainWindow);
	return result;
}

void Presentation_BibleVerse_Wizard::on_btnDone_clicked() {
	accept();
}

void Presentation_BibleVerse_Wizard::on_btnInsert_clicked() {
	if(ui->cbAutoSplit) {
		const BibleRef ref = ui->wgtVerseSelection->bibleRef();
		const auto verses = ref.verses();

		for(int i = 0, e = verses.size(), step = ui->sbAutoSplit->value(); i < e; i += step) {
			addCallback_(BibleRef(ref.translationIds, ref.bookId, ref.chapter, verses.mid(i, step)));
			addCallback_(BibleRef());// Add newline after each verse
		}
	}
	else
		addCallback_(ui->wgtVerseSelection->bibleRef());
}
