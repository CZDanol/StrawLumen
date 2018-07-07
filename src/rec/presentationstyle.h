#ifndef PRESENTATIONSTYLE_H
#define PRESENTATIONSTYLE_H

#include <QObject>

#include "rec/textstyle.h"

class PresentationStyle : public QObject
{
	Q_OBJECT

public:
	explicit PresentationStyle(QObject *parent = nullptr);

signals:
	void sigChanged();

public slots:
	void setMainTextStyle(const TextStyle &style);

public:
	const TextStyle &mainTextStyle() const;

public:
	void drawSlide(QPainter &p, const QRect &rect, const QString &text, const QString &title, const QString &author) const;

public:
	void operator=(const PresentationStyle &other);

private:
	TextStyle mainTextStyle_;

};

#endif // PRESENTATIONSTYLE_H
