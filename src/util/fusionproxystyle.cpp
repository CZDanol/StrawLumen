#include "fusionproxystyle.h"

#include <QStyleFactory>

static const int iconOffset = 3;

FusionProxyStyle::FusionProxyStyle()
{
	setBaseStyle(QStyleFactory::create("Fusion"));
}

// Copied from Qt Github, fixing icon spacing (only small change)

void FusionProxyStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
	if(element == CE_PushButtonLabel) {
		if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
			QRect textRect = button->rect;
			uint textFlags = Qt::AlignVCenter;

			if (styleHint(SH_UnderlineShortcut, button, widget))
					textFlags |= Qt::TextShowMnemonic;
			else
					textFlags |= Qt::TextHideMnemonic;

			if (!button->icon.isNull()) {
					QIcon::Mode mode = button->state & State_Enabled ? QIcon::Normal : QIcon::Disabled;
					if (mode == QIcon::Normal && button->state & State_HasFocus)
							mode = QIcon::Active;

					QIcon::State state = QIcon::Off;
					if (button->state & State_On)
							state = QIcon::On;

					QPixmap pixmap = button->icon.pixmap(button->iconSize, mode, state);
					int w = pixmap.width();
					int h = pixmap.height();

					if (!button->text.isEmpty())
							w += button->fontMetrics.boundingRect(option->rect, textFlags, button->text).width() + 2;

					QPoint point(
								textRect.x() + textRect.width() / 2 - w / 2,
								textRect.y() + textRect.height() / 2 - h / 2);

					// Added this if
					if(!button->text.isEmpty()) {
						point.rx() -= iconOffset;
						textRect.translate(iconOffset*2, 0);
					}

					if (button->direction == Qt::RightToLeft)
							point.rx() += pixmap.width();

					painter->drawPixmap(visualPos(button->direction, button->rect, point), pixmap);

					if (button->direction == Qt::RightToLeft)
							textRect.translate(-point.x() - 2, 0);
					else
							textRect.translate(point.x() + pixmap.width(), 0);

					// left-align text if there is
					if (!button->text.isEmpty())
							textFlags |= Qt::AlignLeft;

			} else {
					textFlags |= Qt::AlignHCenter;
			}

			if (button->features & QStyleOptionButton::HasMenu)
					textRect = textRect.adjusted(0, 0, -proxy()->pixelMetric(PM_MenuButtonIndicator, button, widget), 0);

			proxy()->drawItemText(painter, textRect, textFlags, button->palette, (button->state & State_Enabled), button->text, QPalette::ButtonText);
		}
		return;
	}

	QProxyStyle::drawControl(element, option, painter, widget);
}

QSize FusionProxyStyle::sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
	if(type == CT_PushButton) {
		if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
			QSize result = QProxyStyle::sizeFromContents(type, btn, size, widget);

			if (!btn->icon.isNull())
				result += QSize(iconOffset*2, 0);

			return result;
		}
	}

	return QProxyStyle::sizeFromContents(type, option, size, widget);
}
