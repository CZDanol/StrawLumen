#include "quickbibleversewindow.h"
#include "ui_quickbibleversewindow.h"

#include "gui/mainwindow.h"
#include "gui/mainwindow_presentationmode.h"
#include "presentation/presentationmanager.h"

QuickBibleVerseWindow::QuickBibleVerseWindow(QWidget *parent) : QDialog(parent),
                                                                ui(new Ui::QuickBibleVerseWindow) {
	ui->setupUi(this);

	presentation_ = Presentation_BibleVerse::create();

	connect(ui->wgtVerse, &BibleVerseSelectionWidget::sigSelectionChanged, this, [this] {
		const BibleRef ref = ui->wgtVerse->bibleRef();
		if(!ref.isValid())
			return;

		presentation_->setVersesStr(ref.toString(), true);
		presentationManager->setSlide(presentation_, 0, true);
	});
}

QuickBibleVerseWindow::~QuickBibleVerseWindow() {
	delete ui;
}

QuickBibleVerseWindow *QuickBibleVerseWindow::instance() {
	static QuickBibleVerseWindow *i = new QuickBibleVerseWindow(mainWindow);
	return i;
}

void QuickBibleVerseWindow::showEvent(QShowEvent *e) {
	QDialog::showEvent(e);

	originalPresentationState_.active = presentationManager->isActive();
	originalPresentationState_.slideId = presentationManager->currentGlobalSlideId();

	ui->wgtVerse->setBibleRef({});
	presentation_->setVersesStr({}, true);

	presentationManager->setSlide(presentation_, 0, true);
}

void QuickBibleVerseWindow::closeEvent(QCloseEvent *e) {
	QDialog::closeEvent(e);

	onHideOrClose();
}

void QuickBibleVerseWindow::hideEvent(QHideEvent *e) {
	QDialog::hideEvent(e);

	onHideOrClose();
}

void QuickBibleVerseWindow::onHideOrClose() {
	auto playlist = mainWindow->presentationMode()->playlist();

	if(originalPresentationState_.active && playlist)
		presentationManager->setSlide(playlist.get(), originalPresentationState_.slideId);
	else
		presentationManager->setActive(false);
}
