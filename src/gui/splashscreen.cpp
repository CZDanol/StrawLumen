#include "splashscreen.h"
#include "ui_splashscreen.h"

#include <QCloseEvent>
#include <QEventLoop>

Splashscreen *splashscreen = nullptr;

Splashscreen::Splashscreen(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Splashscreen)
{
	ui->setupUi(this);
	setFixedSize(size());
	setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

	for(int i = 0; i < 40; i ++)
		animImages_.append(QPixmap(QString(":/preloaders/Preloader_4_%1.png").arg(i, 5, 10, QChar('0'))));

	connect(&animTimer_, SIGNAL(timeout()), this, SLOT(onAnimation()));
	animTimer_.setInterval(16);
}

Splashscreen::~Splashscreen()
{
	delete ui;
}

void Splashscreen::asyncAction(const QString &splashMessage, const JobThread::Job &job, JobThread &jobThread)
{
	QEventLoop eventLoop;

	jobThread.executeNonblocking([&]{
		job();
		QMetaObject::invokeMethod(&eventLoop, "quit");
	});

	ui->lblMessage->setText(tr("%1...").arg(splashMessage));
	animTimer_.start();
	show();

	eventLoop.exec();

	canClose_ = true;
	close();
	animTimer_.stop();
}

void Splashscreen::closeEvent(QCloseEvent *e)
{
	if(!canClose_) {
		e->ignore();
		return;
	}

	canClose_ = false;
	QDialog::closeEvent(e);
}

void Splashscreen::onAnimation()
{
	ui->lblAnim->setPixmap(animImages_[animStep_]);

	animStep_ ++;
	if(animStep_ >= animImages_.size())
		animStep_ = 0;
}
