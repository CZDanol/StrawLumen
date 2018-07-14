#include "standarddialogs.h"

#include <QMessageBox>
#include <QPushButton>

#include "util/execonmainthread.h"

bool standardDeleteConfirmDialog(const QString message, QWidget *parent)
{
	QMessageBox *mb = new QMessageBox(QMessageBox::Question, QObject::tr("Potvrdit smazání"), message);
	mb->setParent(parent);
	mb->setWindowModality(Qt::ApplicationModal);

	auto yesBtn = mb->addButton(QObject::tr("Ano"), QMessageBox::YesRole);
	auto noBtn = mb->addButton(QObject::tr("Ne"), QMessageBox::NoRole);

	mb->setEscapeButton(noBtn);
	mb->setDefaultButton(yesBtn);

	static QPixmap iconPixmap(":/icons/32/Trash Can_32px.png");
	mb->setIconPixmap(iconPixmap);

	static QPixmap windowIcon(":/icons/16/Trash Can_32px.png");
	mb->setWindowIcon(windowIcon);

	mb->exec();
	return mb->clickedButton() == yesBtn;
}

bool standardConfirmDialog(const QString message, QWidget *parent)
{
	QMessageBox *mb = new QMessageBox(QMessageBox::Question, QObject::tr("Potvrzení"), message);
	mb->setParent(parent);
	mb->setWindowModality(Qt::ApplicationModal);

	auto yesBtn = mb->addButton(QObject::tr("Ano"), QMessageBox::YesRole);
	auto noBtn = mb->addButton(QObject::tr("Ne"), QMessageBox::NoRole);

	mb->setEscapeButton(noBtn);
	mb->setDefaultButton(yesBtn);

	static QPixmap iconPixmap(":/icons/32/Help_32px.png");
	mb->setIconPixmap(iconPixmap);

	static QPixmap windowIcon(":/icons/16/Help_16px.png");
	mb->setWindowIcon(windowIcon);

	mb->exec();
	return mb->clickedButton() == yesBtn;
}

void standardErrorDialog(const QString message, QWidget *parent, bool blocking)
{
	auto f = [message, parent]{
		QMessageBox *mb = new QMessageBox(QMessageBox::Critical, QObject::tr("Chyba"), message);
		mb->setParent(parent);
		mb->setWindowModality(Qt::ApplicationModal);
		mb->addButton(QObject::tr("Ok"), QMessageBox::AcceptRole);

		static QPixmap iconPixmap(":/icons/32/High Priority_32px.png");
		mb->setIconPixmap(iconPixmap);

		static QPixmap windowIcon(":/icons/16/High Priority_16px.png");
		mb->setWindowIcon(windowIcon);

		mb->exec();
	};

	if(blocking)
		f();
	else
		execOnMainThread(f);
}

void standardInfoDialog(const QString message, QWidget *parent)
{
	execOnMainThread([message, parent]{
		QMessageBox *mb = new QMessageBox(QMessageBox::Information, QObject::tr("Informace"), message);
		mb->setParent(parent);
		mb->setWindowModality(Qt::ApplicationModal);
		mb->addButton(QObject::tr("Ok"), QMessageBox::AcceptRole);

		static QPixmap iconPixmap(":/icons/32/Info_32px.png");
		mb->setIconPixmap(iconPixmap);

		static QPixmap windowIcon(":/icons/16/Info_16px.png");
		mb->setWindowIcon(windowIcon);


		mb->show();
	});
}

void standardSuccessDialog(const QString message, QWidget *parent)
{
	execOnMainThread([message, parent]{
		QMessageBox *mb = new QMessageBox(QMessageBox::Information, QObject::tr("Úspěch"), message);
		mb->setParent(parent);
		mb->setWindowModality(Qt::ApplicationModal);
		mb->addButton(QObject::tr("Ok"), QMessageBox::AcceptRole);

		static QPixmap iconPixmap(":/icons/32/Ok_32px.png");
		mb->setIconPixmap(iconPixmap);

		static QPixmap windowIcon(":/icons/16/Ok_16px.png");
		mb->setWindowIcon(windowIcon);


		mb->show();
	});
}
