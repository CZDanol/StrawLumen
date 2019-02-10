#include "presentationpropertieswidget_images.h"
#include "ui_presentationpropertieswidget_images.h"
#include "presentation_images.h"

#include <QStandardPaths>
#include <QFileDialog>
#include <QShortcut>

#include "job/settings.h"
#include "util/execonmainthread.h"

PresentationPropertiesWidget_Images::PresentationPropertiesWidget_Images(const QSharedPointer<Presentation_Images> &presentation, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PresentationPropertiesWidget_Images),
	presentation_(presentation)
{
	ui->setupUi(this);

	connect(presentation.data(), SIGNAL(sigItemChanged(Presentation*)), this, SLOT(fillData()));

	new QShortcut(Qt::Key_Delete, this, SLOT(deleteSelection()), nullptr, Qt::WidgetWithChildrenShortcut);

	ui->lstImages->viewport()->installEventFilter(this);
	fillData();
}

PresentationPropertiesWidget_Images::~PresentationPropertiesWidget_Images()
{
	delete ui;
}


bool PresentationPropertiesWidget_Images::eventFilter(QObject *obj, QEvent *ev)
{
	Q_UNUSED(obj);

	if(ev->type() == QEvent::Drop) {
		execOnMainThread([this]{
			emit presentation_->sigSlidesChanged();
		});
	}

	return false;
}

void PresentationPropertiesWidget_Images::fillData()
{
	ui->lstImages->setModel(&presentation_->images_);
	ui->cbAutoPresentation->setChecked(presentation_->isAutoPresentation_);
	ui->sbAutoInterval->setValue(presentation_->autoInterval_);
}

void PresentationPropertiesWidget_Images::deleteSelection()
{
	QList<QPersistentModelIndex> lst;
	for(const QModelIndex &mi : ui->lstImages->selectionModel()->selectedRows())
		lst += mi;

	for(auto mi: lst)
		presentation_->images_.removeRow(mi.row());

	emit presentation_->sigSlidesChanged();
}

void PresentationPropertiesWidget_Images::on_btnAddItems_clicked()
{
	static const QIcon icon(":/icons/16/Add_16px.png");

	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFiles);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setNameFilter(tr("Soubory obrázků (%1)").arg("*." + Presentation_Images::validExtensions().join(" *.")));
	dlg.setWindowIcon(icon);
	dlg.setWindowTitle(tr("Přidání obrázků"));
	dlg.setDirectory(settings->value("imagesDirectory", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString());

	if(!dlg.exec())
		return;

	settings->setValue("imagesDirectory", dlg.directory().absolutePath());
	presentation_->addImages(dlg.selectedFiles());
}

void PresentationPropertiesWidget_Images::on_cbAutoPresentation_clicked(bool checked)
{
	presentation_->isAutoPresentation_ = checked;
	presentation_->updateTiming();

	emit presentation_->sigSlidesChanged();
	emit presentation_->sigItemChanged(presentation_.data());
}

void PresentationPropertiesWidget_Images::on_sbAutoInterval_valueChanged(int arg1)
{
	presentation_->autoInterval_ = arg1;
	presentation_->updateTiming();

	emit presentation_->sigChanged();
}
