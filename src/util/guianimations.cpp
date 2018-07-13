#include "guianimations.h"

#include <QTimer>
#include <QPointer>
#include <QPalette>

void blinkButton(QPushButton *btn)
{
	static auto func = [](QPushButton *btn, int offset) {
		static QColor blinkColor("#ffa632");

		QPointer<QPushButton> ptr(btn);
		QTimer::singleShot(offset, [ptr]{
			if(ptr.isNull())
				return;

			const bool wasFlat = ptr->isFlat();
			ptr->setFlat(false);

			const QPalette originalPalette = ptr->palette();
			QPalette palette = originalPalette;
			palette.setColor(QPalette::Button, blinkColor);

			ptr->setPalette(palette);
			QTimer::singleShot(300, [ptr, wasFlat, originalPalette]{
				if(ptr.isNull())
					return;

				ptr->setPalette(originalPalette);
				ptr->setFlat(wasFlat);
			});
		});
	};

	func(btn, 0);
	func(btn, 600);
}
