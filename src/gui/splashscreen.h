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
	explicit Splashscreen(QWidget *parent = nullptr);
	virtual ~Splashscreen() override;

public:
	void show(const QString &splashMessage, bool enableStorno);
	void close();

	/// Blockingly shows the splashscreen until the job is done
	void asyncAction(const QString &splashMessage, bool enableStorno, JobThread &jobThread, const JobThread::Job &job);

	/// Blockingly shows the splashscreen until the job is done / uses QtConcurrent::run
	void asyncAction(const QString &splashMessage, bool enableStorno, const JobThread::Job &job);

	void setProgress(int val, int max);

	bool isStornoPressed();

public slots:
	void storno();

protected:
	void reject() override;

private:
	void _asyncAction(const QString &splashMessage, bool enableStorno, const JobThread::Job &job, const std::function<void(const JobThread::Job &)> &issueFunc);

private slots:
	void onAnimation();

private:
	Ui::Splashscreen *ui;
	bool canClose_ = false;

private:
	QVector<QPixmap> animImages_;
	int animStep_ = 0;
	QTimer animTimer_;
	bool isStornoPressed_ = false;

};

extern Splashscreen *splashscreen;

#endif // SPLASHSCREEN_H
