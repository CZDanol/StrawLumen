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

private:
	TextStyle mainTextStyle_;

};

#endif // PRESENTATIONSTYLE_H
