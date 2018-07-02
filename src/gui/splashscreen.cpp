#include "splashscreen.h"
#include "ui_splashscreen.h"

#include <QCloseEvent>
#include <QEventLoop>

#include "util/execonmainthread.h"

Splashscreen *splashscreen = nullptr;

Splashscreen::Splashscreen(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Splashscreen)
{
	ui->setupUi(this);
	setFixedSize(size());
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	setWindowTitle(" ");

	for(int i = 0; i < 40; i ++)
		animImages_.append(QPixmap(QString(":/preloaders/Preloader_4_%1.png").arg(i, 5, 10, QChar('0'))));

	connect(&animTimer_, SIGNAL(timeout()), this, SLOT(onAnimation()));
	animTimer_.setInterval(16);
}

Splashscreen::~Splashscreen()
{
	delete ui;
}

void Splashscreen::asyncAction(const QString &splashMessage, bool enableStorno, JobThread &jobThread, const JobThread::Job &job)
{
	QEventLoop eventLoop;

	jobThread.executeNonblocking([&]{
		job();
		QMetaObject::invokeMethod(&eventLoop, "quit");
	});

	ui->lblMessage->setText(tr("%1...").arg(splashMessage));
	animTimer_.start();

	ui->wgtStack->setCurrentWidget(ui->wgtPageAnim);
	isStornoPressed_ = false;

	setWindowFlag(Qt::WindowCloseButtonHint, enableStorno);
	show();

	eventLoop.exec();

	canClose_ = true;
	close();
	animTimer_.stop();
}

void Splashscreen::setProgress(int val, int max)
{
	execOnMainThread([=]{
		ui->pbProgress->setMaximum(max);
		ui->pbProgress->setValue(val);

		ui->wgtStack->setCurrentWidget(max == 0 ? ui->wgtPageAnim : ui->wgtPageProgress);
	});
}

bool Splashscreen::isStornoPressed()
{
	return isStornoPressed_;
}

void Splashscreen::reject()
{
	if(!canClose_) {
		isStornoPressed_ = true;
		return;
	}

	canClose_ = false;
	QDialog::reject();
}

void Splashscreen::onAnimation()
{
	ui->lblAnim->setPixmap(animImages_[animStep_]);

	animStep_ ++;
	if(animStep_ >= animImages_.size())
		animStep_ = 0;
}
