#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAxObject>
#include <QDebug>

MainWindow *mainWindow = nullptr;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setWindowTitle(tr("Straw Lumen v%1").arg(PROGRAM_VERSION));

	/*ui->axw->setControl("D:/Tvorba/Qt/Straw Organ/bin/bin_x86_debug/prez.pptx");
	ui->axw->dynamicCall("Run()");*/

	/*QAxObject obj("PowerPoint.Application");
	auto presentations = obj.querySubObject("Presentations");
	auto presentation = presentations->querySubObject("Open(QString,Office::MsoTriState,Office::MsoTriState,Office::MsoTriState)", "D:\\Tvorba\\Qt\\Straw Organ\\bin\\bin_x86_debug\\prez.pptx", true, false, false);
	auto slides = presentation->querySubObject("Slides");

	auto ssSettings = presentation->querySubObject("SlideShowSettings");
	//ui->tb->setHtml(ssSettings->generateDocumentation());
	//ssSettings->dynamicCall("SetShowPresenterView(Office::MsoTriState)", false);
	ssSettings->dynamicCall("SetShowType(Office::PpSlideShowType)", "ppShowTypeKiosk");
	ssSettings->dynamicCall("Run()");*/


	/*int numSlides = slides->property("Count").toInt();
	for(int i = 1; i <= numSlides; i++) {
		auto slide = slides->querySubObject("Item(QVariant)", i);

		auto shapes = slide->querySubObject("Shapes");
		int numShapes = shapes->property("Count").toInt();
		for(int j = 1; j < numShapes; j++) {
			auto shape = shapes->querySubObject("Item(QVariant)", j);
			auto textFrame = shape->querySubObject("TextFrame");

			if(!textFrame)
				continue;

			auto textRange = textFrame->querySubObject("TextRange");

			qDebug() << textRange->property("Text").toString();
		}
	}*/

	//obj.dynamicCall("Quit()");
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButton_clicked()
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
}
