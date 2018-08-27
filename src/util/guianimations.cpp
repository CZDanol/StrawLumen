#include "guianimations.h"

#include <functional>

#include <QTimer>
#include <QPointer>
#include <QPalette>
#include <QSharedPointer>
#include <QDateTime>
#include <QtMath>

static const QColor flashColor("#ffa632");

QColor mixColors(const QColor &c1, const QColor &c2, qreal progress) {
	const qreal progressInv = 0.99 - progress;
	return QColor::fromRgbF(
				c1.redF() * progressInv + c2.redF() * progress,
				c1.greenF() * progressInv + c2.greenF() * progress,
				c1.blueF() * progressInv + c2.blueF() * progress
				);
}

auto flashAnimation(QWidget *wgt, std::function<void()> endFunc) {
	const qint64 animStart = QDateTime::currentMSecsSinceEpoch();
	const QPalette originalPalette = wgt->palette();

	QTimer *tim = new QTimer(wgt);
	QObject::connect(tim, &QTimer::timeout, [animStart, wgt, originalPalette, tim, endFunc]{
		const qint64 msecsElapsed = QDateTime::currentMSecsSinceEpoch() - animStart;
		const qreal progress = abs(sin(msecsElapsed / 200.0 * M_PI_2));
		QPalette palette = originalPalette;
		palette.setColor(QPalette::Window, mixColors(originalPalette.color(QPalette::Window), flashColor, progress));
		palette.setColor(QPalette::Base, mixColors(originalPalette.color(QPalette::Base), flashColor, progress));
		palette.setColor(QPalette::AlternateBase, mixColors(originalPalette.color(QPalette::AlternateBase), flashColor, progress));
		palette.setColor(QPalette::Button, mixColors(originalPalette.color(QPalette::Button), flashColor, progress));
		palette.setColor(QPalette::Light, mixColors(originalPalette.color(QPalette::Light), flashColor, progress));
		palette.setColor(QPalette::Midlight, mixColors(originalPalette.color(QPalette::Midlight), flashColor, progress));
		palette.setColor(QPalette::Dark, mixColors(originalPalette.color(QPalette::Dark), flashColor, progress));
		palette.setColor(QPalette::Mid, mixColors(originalPalette.color(QPalette::Mid), flashColor, progress));

		wgt->setPalette(palette);

		if(msecsElapsed > 800) {
			wgt->setPalette(originalPalette);
			endFunc();
			tim->deleteLater();
		}
	});

	tim->setInterval(16);
	tim->start();
}

void flashButton(QPushButton *btn)
{
	const bool wasFlat = btn->isFlat();
	btn->setFlat(false);

	flashAnimation(btn, [btn, wasFlat]{
		btn->setFlat(wasFlat);
	});
}

void flashWidget(QWidget *wgt)
{
	flashAnimation(wgt, []{});
}
