#ifndef PRESENTATIONBACKGROUND_H
#define PRESENTATIONBACKGROUND_H

#include <QColor>
#include <QCoreApplication>
#include <QJsonValue>
#include <QPainter>
#include <QString>

class PresentationBackground : public QObject {
	Q_OBJECT

public:
	PresentationBackground();
	PresentationBackground(const PresentationBackground &other);

signals:
	void sigChanged();
	void sigNeedsRepaint();

public:
	QString caption() const;

	const QString &filename() const;
	const QColor &color() const;
	int blendMode() const;

public slots:
	void setFilename(const QString &filename);
	void setColor(const QColor &color);
	void setBlendMode(int blendMode);

public:
	void draw(QPainter &p, const QRect &rect) const;

public:
	QJsonValue toJSON() const;
	void loadFromJSON(const QJsonValue &json);

public:
	bool operator==(const PresentationBackground &other) const;

	void operator=(const PresentationBackground &other);

private slots:
	void onBackgroundManagerBackgroundLoaded(const QString &filename);

private:
	QString filename_;
	QColor color_ = Qt::transparent;
	int blendMode_ = QPainter::CompositionMode_SourceOver;
};

#endif// PRESENTATIONBACKGROUND_H
