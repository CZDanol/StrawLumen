#include "presentationpropertieswidget_video.h"
#include "ui_presentationpropertieswidget_video.h"

PresentationPropertiesWidget_Video::PresentationPropertiesWidget_Video(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PresentationPropertiesWidget_Video)
{
	ui->setupUi(this);
}

PresentationPropertiesWidget_Video::~PresentationPropertiesWidget_Video()
{
	delete ui;
}
