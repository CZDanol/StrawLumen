#include "displayselectionwidget.h"
#include "ui_displayselectionwidget.h"

#include <QGuiApplication>

#include "gui/mainwindow.h"
#include "gui/splashscreen.h"

DisplaySelectionWidget::DisplaySelectionWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::DisplaySelectionWidget)
{
	ui->setupUi(this);

	connect(ui->cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));

	updateScreenList();
}

DisplaySelectionWidget::~DisplaySelectionWidget()
{
	delete ui;
}

QScreen *DisplaySelectionWidget::selectedScreen() const
{
	return ui->cmb->currentIndex() == -1 ? primaryScreen_ : screenList_[ui->cmb->currentIndex()];
}

void DisplaySelectionWidget::updateScreenList()
{
	ui->cmb->clear();

	screenList_ = QGuiApplication::screens();
	primaryScreen_ = QGuiApplication::primaryScreen();
	QPoint primaryScreenCenter = primaryScreen_->geometry().center();

	for(int i = 0; i < screenList_.size(); i ++) {
		QScreen *screen = screenList_[i];
		QString positionStr;

		if(screen == primaryScreen_)
			positionStr = tr("hlavní");

		else {
			QPoint center = screen->geometry().center();
			if(abs(center.x() - primaryScreenCenter.x()) > abs(center.y() - primaryScreenCenter.y())) {
				if(center.x() > primaryScreenCenter.x())
					positionStr = tr("vpravo");
				else
					positionStr = tr("vlevo");
			} else {
				if(center.y() > primaryScreenCenter.y())
					positionStr = tr("dole");
				else
					positionStr = tr("nahoře");
			}
		}

		ui->cmb->addItem(tr("Obrazovka %1 (%2)").arg(i+1).arg(positionStr));
	}
}

void DisplaySelectionWidget::onCurrentIndexChanged(int current)
{
	if(current == -1)
		return;

	emit sigCurrentChanged(selectedScreen());
}
