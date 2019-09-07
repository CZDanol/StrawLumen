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

	connect(ui->cmb, SIGNAL(activated(int)), this, SLOT(onItemActivated(int)));
	connect(qApp, &QApplication::screenAdded, this, &DisplaySelectionWidget::updateScreenList);
	connect(qApp, &QApplication::screenRemoved, this, &DisplaySelectionWidget::updateScreenList);

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

QPair<QRect, QString> DisplaySelectionWidget::selectedScreenId() const
{
	auto screen = selectedScreen();
	return QPair<QRect, QString>(screen->geometry(), screen->name());
}

void DisplaySelectionWidget::setSelectedScreen(const QPair<QRect, QString> &id)
{
	int bestCoverage = 0;
	int bestIndex = -1;

	for(int i = 0; i < screenList_.size(); i++) {
		QScreen *screen = screenList_[i];

		// Name match - use that
		if(screen->name() == id.second) {
			ui->cmb->setCurrentIndex(i);
			return;
		}

		const QRect intersection = screen->geometry().intersected(id.first);
		const int coverage = intersection.width() * intersection.height();

		if(coverage > bestCoverage) {
			bestIndex = i;
			bestCoverage = coverage;
		}
	}

	// Best coverage - use that
	if(bestIndex != -1) {
		ui->cmb->setCurrentIndex(bestIndex);
		return;
	}

	ui->cmb->setCurrentIndex(isPreferNonprimaryScreens_ ? anyNonprimaryScreenIndex_ : primaryScreenIndex_);
}

void DisplaySelectionWidget::updateScreenList()
{
	ui->cmb->clear();

	screenList_ = QGuiApplication::screens();
	primaryScreen_ = QGuiApplication::primaryScreen();
	QPoint primaryScreenCenter = primaryScreen_->geometry().center();

	anyNonprimaryScreenIndex_ = 0;

	for(int i = 0; i < screenList_.size(); i ++) {
		QScreen *screen = screenList_[i];
		QString positionStr;

		if(screen == primaryScreen_) {
			positionStr = tr("hlavní");
			primaryScreenIndex_ = i;
		}

		else {
			anyNonprimaryScreenIndex_ = i;
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

void DisplaySelectionWidget::onItemActivated(int current)
{
	if(current == -1)
		return;

	emit sigCurrentChangedByUser(selectedScreen());
}
