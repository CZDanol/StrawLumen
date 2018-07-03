#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "splashscreen.h"

MainWindow *mainWindow = nullptr;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->swMenu->addWidget(ui->wgtPresentationMode->menuWidget());

	setWindowTitle(tr("Straw Lumen v%1").arg(PROGRAM_VERSION));
}

MainWindow::~MainWindow()
{
	delete ui;
}

/*void MainWindow::on_pushButton_clicked()
{
	obj.setControl("PowerPoint.Application");
	auto presentations = obj.querySubObject("Presentations");
	presentation = presentations->querySubObject("Open(QString,Office::MsoTriState,Office::MsoTriState,Office::MsoTriState)", "D:\\Tvorba\\Qt\\Straw Organ\\bin\\bin_x86_debug\\prez.pptx", true, false, false);
	auto slides = presentation->querySubObject("Slides");

	auto ssSettings = presentation->querySubObject("SlideShowSettings");
	//ui->tb->setHtml(ssSettings->generateDocumentation());
	//ssSettings->dynamicCall("SetShowPresenterView(Office::MsoTriState)", false);
	ssSettings->dynamicCall("SetShowType(Office::PpSlideShowType)", "ppShowTypeKiosk");
	ssSettings->dynamicCall("Run()");
}

void MainWindow::on_pushButton_2_clicked()
{
	auto ssWindow = presentation->querySubObject("SlideShowWindow");
	//ui->tb->setHtml(ssWindow->generateDocumentation());
	ssWindow->dynamicCall("Activate()");

	//ssWindow->dynamicCall("SetState(Office::PpSlideShowState)","ppSlideShowRunning");
}*/
