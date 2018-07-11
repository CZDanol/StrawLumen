#include "multisongselectionwidget.h"
#include "ui_multisongselectionwidget.h"

MultiSongSelectionWidget::MultiSongSelectionWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MultiSongSelectionWidget)
{
	ui->setupUi(this);
	ui->twSongs->setCornerWidget(ui->twSongsCorner);
	ui->twSongbook->setCornerWidget(ui->twSongbookCorner);
}

MultiSongSelectionWidget::~MultiSongSelectionWidget()
{
	delete ui;
}
