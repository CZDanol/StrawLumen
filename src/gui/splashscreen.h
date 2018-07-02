#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QDialog>
#include <QVector>
#include <QPixmap>
#include <QTimer>

#include "util/jobthread.h"

namespace Ui {
	class Splashscreen;
}

class Splashscreen : public QDialog
{
	Q_OBJECT

public:
	explicit Splashscreen(QWidget *parent = 0);
	~Splashscreen();

public:
	/// Blockingly shows the splashscreen until the job is done
	void asyncAction(const QString &splashMessage, bool enableStorno, JobThread &jobThread, const JobThread::Job &job);

	void setProgress(int val, int max);

	bool isStornoPressed();

protected:
	void reject() override;

private slots:
	void onAnimation();

private:
	Ui::Splashscreen *ui;
	bool canClose_ = false;

private:
	QVector<QPixmap> animImages_;
	int animStep_ = 0;
	QTimer animTimer_;
	bool isStornoPressed_;

};

extern Splashscreen *splashscreen;

#endif // SPLASHSCREEN_H
