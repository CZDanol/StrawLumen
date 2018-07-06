#ifndef TEXTSTYLE_H
#define TEXTSTYLE_H

#include <QFont>
#include <QColor>

struct TextStyle {

public:
	QFont font = QFont("Tahoma", 12);
	QColor color = Qt::black;

public:
	bool outlineEnabled = false;
	int outlineWidth = 1;
	QColor outlineColor = Qt::white;

public:
	bool backgroundEnabled = false;
	int backgroundPadding = 0;
	QColor backgroundColor = QColor(0,0,0,128);

public:
	bool operator==(const TextStyle &other) const;

};

#endif // TEXTSTYLE_H
