#include "bibletranslationmgmtdialog.h"
#include "ui_bibletranslationmgmtdialog.h"

#include <QFileDialog>

#include "job/db.h"
#include "gui/mainwindow.h"
#include "gui/splashscreen.h"
#include "util/updatesdisabler.h"
#include "util/standarddialogs.h"
#include "job/parsebible.h"

BibleTranslationMgmtDialog::BibleTranslationMgmtDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::BibleTranslationMgmtDialog)
{
	ui->setupUi(this);
	ui->twList->setCornerWidget(ui->twListCorner);

	connect(db, &DatabaseManager::sigBibleTranslationsChanged, this, &BibleTranslationMgmtDialog::updateList);
	connect(ui->lstList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &BibleTranslationMgmtDialog::updateUiEnabled);

	updateList();
	updateUiEnabled();
}

BibleTranslationMgmtDialog::~BibleTranslationMgmtDialog()
{
	delete ui;
}

void BibleTranslationMgmtDialog::updateList()
{
	UpdatesDisabler _ud(ui->lstList);
	ui->lstList->clear();

	QSqlQuery q = db->selectQuery("SELECT translation_id, name FROM bible_translations ORDER BY translation_id");
	QList<QTreeWidgetItem*> items;
	while(q.next()) {
		QTreeWidgetItem *i = new QTreeWidgetItem();
		i->setText(0, q.value(0).toString());
		i->setText(1, q.value(1).toString());
		items += i;
	}

	ui->lstList->addTopLevelItems(items);
}

void BibleTranslationMgmtDialog::updateUiEnabled()
{
	ui->btnDelete->setEnabled(!ui->lstList->selectedItems().isEmpty());
}

BibleTranslationMgmtDialog *bibleTranslationMgmtDialog()
{
	static BibleTranslationMgmtDialog *result = new BibleTranslationMgmtDialog(mainWindow);
	return result;
}

void BibleTranslationMgmtDialog::on_btnDelete_clicked()
{
	if(!standardDeleteConfirmDialog(tr("Opravdu smazat vybrané překlady?"), this))
		return;

	QStringList translations;
	for(auto i : ui->lstList->selectedItems())
		translations += i->text(0);

	splashscreen->asyncAction(tr("Mazání překladů"), false, [&] {
		for(const auto &translation : translations)
			deleteBible(translation);
	});

	emit db->sigBibleTranslationsChanged();
}

void BibleTranslationMgmtDialog::on_btnImport_clicked()
{
	static const QIcon icon(":/icons/16/Holy Bible_16px.png");

	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFiles);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setNameFilter(tr("Zefania XML Bible (*.xml)"));
	dlg.setWindowIcon(icon);
	dlg.setWindowTitle(tr("Import překladu Bible"));

	if(!dlg.exec())
		return;

	splashscreen->asyncAction(QObject::tr("Import překladu Bible"), false, [&]{
		for(auto file : dlg.selectedFiles()) {
			if(!parseBible(file))
				return;
		}

		standardInfoDialog(tr("Překlady úspěšně importovány."));
	});
}
