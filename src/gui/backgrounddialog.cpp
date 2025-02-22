#include "backgrounddialog.h"
#include "ui_backgrounddialog.h"

#include <QApplication>
#include <QBuffer>
#include <QDirIterator>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QMimeData>
#include <QPainter>
#include <QShortcut>
#include <QStandardPaths>
#include <QUrl>

#include "gui/splashscreen.h"
#include "job/backgroundmanager.h"
#include "job/settings.h"
#include "main.h"
#include "util/execonmainthread.h"
#include "util/standarddialogs.h"

BackgroundDialog *backgroundDialog = nullptr;

enum ItemDataRole {
	idrFilename = Qt::UserRole,
	idrFilePath,
	idrThumbnailFilePath,
	idrIsIntegratedBackground
};

static const QList<QPair<const char *, int>> blendModes{
  {QT_TRANSLATE_NOOP(BackgroundDialog, "Překrýt (klasické)"), QPainter::CompositionMode_SourceOver},
  {QT_TRANSLATE_NOOP(BackgroundDialog, "Multiply"), QPainter::CompositionMode_Multiply},
  {QT_TRANSLATE_NOOP(BackgroundDialog, "Screen"), QPainter::CompositionMode_Screen},
  {QT_TRANSLATE_NOOP(BackgroundDialog, "Overlay"), QPainter::CompositionMode_Overlay},
  {QT_TRANSLATE_NOOP(BackgroundDialog, "Darken"), QPainter::CompositionMode_Darken},
  {QT_TRANSLATE_NOOP(BackgroundDialog, "Lighten"), QPainter::CompositionMode_Lighten},
  {QT_TRANSLATE_NOOP(BackgroundDialog, "Color dodge"), QPainter::CompositionMode_ColorDodge},
  {QT_TRANSLATE_NOOP(BackgroundDialog, "Color burn"), QPainter::CompositionMode_ColorBurn},
  {QT_TRANSLATE_NOOP(BackgroundDialog, "Hard light"), QPainter::CompositionMode_HardLight},
  {QT_TRANSLATE_NOOP(BackgroundDialog, "Soft light"), QPainter::CompositionMode_SoftLight}};

static const QHash<int, int> blendModeIndexes = []() {
	QHash<int, int> result;
	for(int i = 0; i < blendModes.size(); i++)
		result.insert(blendModes[i].second, i);

	return result;
}();

BackgroundDialog::BackgroundDialog(QWidget *parent) : QDialog(parent),
                                                      ui(new Ui::BackgroundDialog) {
	ui->setupUi(this);
	ui->twGallery->setCornerWidget(ui->twGalleryCorner);
	ui->twColor->setCornerWidget(ui->twColorCorner);

	for(const auto &bm: blendModes)
		ui->cmbBlendMode->addItem(tr(bm.first), bm.second);

	connect(ui->btnClose, SIGNAL(clicked(bool)), this, SLOT(accept()));
	connect(ui->btnSelect, SIGNAL(clicked(bool)), this, SLOT(accept()));
	connect(ui->btnStorno, SIGNAL(clicked(bool)), this, SLOT(reject()));
	connect(ui->lwList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onLwGalleryContextMenuRequested(QPoint)));

	connect(new QShortcut(Qt::Key_Delete, this, nullptr, nullptr, Qt::WidgetWithChildrenShortcut), SIGNAL(activated()), ui->actionDelete, SLOT(trigger()));

	galleryContextMenu_ = new QMenu(this);
	galleryContextMenu_->addAction(ui->actionDelete);

	backgroundsDirectory_ = QDir(appDataDirectory.absoluteFilePath("backgrounds"));
}

BackgroundDialog::~BackgroundDialog() {
	delete ui;
}

void BackgroundDialog::showInMgmtMode() {
	setMgmtMode(true);
	ui->lwList->setCurrentRow(0);
	QDialog::show();
}

bool BackgroundDialog::showInSelectionMode(PresentationBackground &background) {
	presentationBackground_ = background;
	ui->lwList->setCurrentItem(itemsByFilename_.value(presentationBackground_.filename(), nullptr));
	ui->wgtColor->setColor(presentationBackground_.color());
	ui->twGallery->setCurrentIndex(0);
	ui->cmbBlendMode->setCurrentIndex(blendModeIndexes.value(presentationBackground_.blendMode(), 0));

	setMgmtMode(false);
	updatePreview();

	const bool accepted = QDialog::exec() == QDialog::Accepted;
	if(accepted)
		background = presentationBackground_;

	return accepted;
}

const QDir &BackgroundDialog::backgroundsDirectory() const {
	return backgroundsDirectory_;
}

void BackgroundDialog::dragEnterEvent(QDragEnterEvent *e) {
	if(e->mimeData()->hasUrls()) {
		for(QUrl url: e->mimeData()->urls()) {
			if(!url.isLocalFile())
				return;
		}

		e->setDropAction(Qt::LinkAction);
		e->accept();
	}
}

void BackgroundDialog::dropEvent(QDropEvent *e) {
	auto urls = e->mimeData()->urls();

	// The exec is there so the explorer the file is dragged from is not frozen while loading
	execOnMainThread([=] {
		for(QUrl url: urls) {
			if(!addBackground(url.toLocalFile()))
				return;
		}
	});
}

void BackgroundDialog::showEvent(QShowEvent *e) {
	loadBackgrounds();
	QDialog::showEvent(e);
}

void BackgroundDialog::loadBackgrounds(bool force) {
	if(!force && backgroundsLoaded_)
		return;

	struct Item {
		QString filename;
		QPixmap thumbnail;
	};

	QVector<QListWidgetItem *> data;

	splashscreen->asyncAction(tr("Načítání pozadí"), false, [&] {
		auto addFromDirectory = [&](const QDir &dir, bool isIntegrated) {
			QDirIterator it(dir.absolutePath(), {"*.full.*"}, QDir::Files | QDir::Readable);
			while(it.hasNext()) {
				it.next();

				const QString thumbnailFilePath = dir.absoluteFilePath(it.fileInfo().baseName() + ".thumbnail.png");
				QImage thumbnail(thumbnailFilePath);

				if(thumbnail.isNull())
					continue;

				QListWidgetItem *i = new QListWidgetItem();
				i->setData(idrFilename, it.fileName());
				i->setData(idrFilePath, it.fileInfo().absoluteFilePath());
				i->setData(idrThumbnailFilePath, thumbnailFilePath);
				i->setData(idrIsIntegratedBackground, isIntegrated);
				i->setIcon(QPixmap::fromImage(thumbnail));
				data.append(i);
			}
		};

		addFromDirectory(backgroundManager->userBackgroundDirectory().absolutePath(), false);
		addFromDirectory(backgroundManager->internalBackgroundDirectory().absolutePath(), true);
	});

	for(QListWidgetItem *item: data) {
		itemsByFilename_.insert(item->data(idrFilename).toString(), item);
		ui->lwList->addItem(item);
	}

	backgroundsLoaded_ = true;
}

bool BackgroundDialog::addBackground(const QString &filename) {
	QListWidgetItem *result = nullptr;

	splashscreen->asyncAction(tr("Načítání \"%1\"").arg(filename), true, [&] {
		QImage fullImage, thumbnail;

		// Load original file
		{
			if(!fullImage.load(filename))
				return standardErrorDialog(tr("Pozadí \"%1\" se nepodařilo načíst.").arg(filename));

			if(splashscreen->isStornoPressed())
				return;
		}

		// Generate thumbnail
		{
			thumbnail = QImage(BACKGROUND_THUMBNAIL_SIZE, BACKGROUND_THUMBNAIL_SIZE, QImage::Format_ARGB32);
			thumbnail.fill(Qt::transparent);

			QRect targetRect;
			targetRect.setSize(fullImage.size().scaled(thumbnail.size(), Qt::KeepAspectRatio));
			targetRect.moveCenter(thumbnail.rect().center());

			QPainter p(&thumbnail);
			p.drawImage(targetRect, fullImage.scaled(targetRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

			if(splashscreen->isStornoPressed())
				return;
		}

		const int i = settings->value("backgrounds.idIncrement").toInt() + 1;
		settings->setValue("backgrounds.idIncrement", i);

		const QString newFilePathBase = backgroundsDirectory_.absoluteFilePath(QString::number(i).rightJustified(4, '0'));
		const QString newFilePath = newFilePathBase + ".full." + QFileInfo(filename).suffix();
		const QString thumbnailFilePath = newFilePathBase + ".thumbnail.png";

		// Copy background file
		{
			if(!QFile::copy(filename, newFilePath))
				return standardErrorDialog(tr("Pozadí se nepodařilo vložit (kopírování selhalo)"));

			if(splashscreen->isStornoPressed())
				return;
		}

		// Save thumbnail
		{
			if(!thumbnail.save(thumbnailFilePath)) {
				QFile(newFilePath).remove();
				return standardErrorDialog(tr("Pozadí se nepodařilo vložit (ukládání náhledu selhalo)"));
			}

			if(splashscreen->isStornoPressed())
				return;
		}

		// Create item
		{
			result = new QListWidgetItem();
			result->setIcon(QPixmap::fromImage(thumbnail));
			result->setData(idrFilename, QFileInfo(newFilePath).fileName());
			result->setData(idrFilePath, newFilePath);
			result->setData(idrThumbnailFilePath, thumbnailFilePath);
			result->setData(idrIsIntegratedBackground, false);
		}
	});

	if(result) {
		itemsByFilename_.insert(result->data(idrFilename).toString(), result);
		ui->lwList->addItem(result);
	}

	return result != nullptr;
}

void BackgroundDialog::setMgmtMode(bool set) {
	ui->btnStorno->setVisible(!set);
	ui->btnSelect->setVisible(!set);
	ui->btnClose->setVisible(set);

	ui->twColor->setVisible(!set);
	ui->twPreview->setVisible(!set);

	isMgmtMode_ = set;
}

void BackgroundDialog::updatePreview() {
	if(isMgmtMode_)
		return;

	ui->wgtPreview->setPresentationBackground(presentationBackground_);
}

void BackgroundDialog::onLwGalleryContextMenuRequested(const QPoint &pos) {
	if(!ui->lwList->currentItem())
		return;

	galleryContextMenu_->popup(ui->lwList->viewport()->mapToGlobal(pos));
}

void BackgroundDialog::on_btnAdd_clicked() {
	static const QIcon icon(":/icons/16/Full Image_16px.png");

	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFiles);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setNameFilter(tr("Soubory obrázků (*.jpg *.jpeg *.png *.bmp *.gif)"));
	dlg.setWindowIcon(icon);
	dlg.setWindowTitle(tr("Import pozadí"));

	QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
	dlg.setDirectory(settings->value("dialog.importBackground.directory", dirs.length() ? dirs.first() : "").toString());

	if(!dlg.exec())
		return;

	settings->setValue("dialog.importBackground.directory", dlg.directory().absolutePath());

	for(auto &filename: dlg.selectedFiles()) {
		if(!addBackground(filename))
			break;
	}
}

void BackgroundDialog::on_lwList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *) {
	ui->actionDelete->setEnabled(current && !current->data(idrIsIntegratedBackground).toBool());

	const QString filename = current ? current->data(idrFilename).toString() : QString();

	if(filename == presentationBackground_.filename())
		return;

	presentationBackground_.setFilename(filename);
	updatePreview();
}

void BackgroundDialog::on_actionDelete_triggered() {
	QListWidgetItem *item = ui->lwList->currentItem();
	if(!item || item->data(idrIsIntegratedBackground).toBool())
		return;

	if(!standardDeleteConfirmDialog(tr("Opravdu smazat vybrané pozadí?")))
		return;

	QFile(item->data(idrFilePath).toString()).remove();
	QFile(item->data(idrThumbnailFilePath).toString()).remove();
	delete item;
}

void BackgroundDialog::on_wgtColor_sigColorChangedByUser(const QColor &newColor) {
	presentationBackground_.setColor(newColor);
	updatePreview();
}

void BackgroundDialog::on_lwList_itemActivated(QListWidgetItem *item) {
	Q_UNUSED(item);
	if(!isMgmtMode_)
		ui->btnSelect->click();
}

void BackgroundDialog::on_cmbBlendMode_activated(int index) {
	presentationBackground_.setBlendMode(ui->cmbBlendMode->itemData(index).toInt());
	updatePreview();
}

void BackgroundDialog::on_btnResetColor_clicked() {
	presentationBackground_.setColor(Qt::transparent);
	presentationBackground_.setBlendMode(QPainter::CompositionMode_SourceOver);

	ui->wgtColor->setColor(Qt::transparent);
	ui->cmbBlendMode->setCurrentIndex(0);

	updatePreview();
}
