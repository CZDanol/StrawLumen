#ifndef PRESENTATIONBACKGROUND_H
#define PRESENTATIONBACKGROUND_H

#include <QString>
#include <QColor>
#include <QJsonValue>
#include <QCoreApplication>
#include <QPainter>

class PresentationBackground
{
	Q_DECLARE_TR_FUNCTIONS(PresentationBackground)

public:
	PresentationBackground();

public:
	QString filename;
	QColor color = Qt::transparent;

public:
	QString caption() const;

	void draw(QPainter &p, const QRect &rect) const;

public:
	QJsonValue toJSON() const;
	void loadFromJSON(const QJsonValue &json);

public:
	bool operator==(const PresentationBackground &other) const;

};

#endif // PRESENTATIONBACKGROUND_H
