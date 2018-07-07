#ifndef TEXTSTYLE_H
#define TEXTSTYLE_H

#include <QFont>
#include <QColor>

struct TextStyle {

public:
	QFont font = QFont("Tahoma", 12);
	QColor color = Qt::white;

public:
	bool outlineEnabled = false;
	int outlineWidth = 8;
	QColor outlineColor = Qt::black;

public:
	bool backgroundEnabled = false;
	int backgroundPadding = 20;
	QColor backgroundColor = QColor(0,0,0,128);

public:
	bool operator==(const TextStyle &other) const;

};

#endif // TEXTSTYLE_H
