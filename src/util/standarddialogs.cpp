#include "standarddialogs.h"

#include <QMessageBox>
#include <QPushButton>

#include "util/execonmainthread.h"

bool deleteConfirmDialog(const QString message, QWidget *parent)
{
	QMessageBox *mb = new QMessageBox( QMessageBox::Question, QObject::tr("Potvrdit smazání"), message );
	mb->setParent( parent );
	mb->setWindowModality( Qt::ApplicationModal );

	mb->addButton( QObject::tr("Ano"), QMessageBox::YesRole );
	auto noBtn = mb->addButton( QObject::tr("Ne"), QMessageBox::NoRole );

	mb->setEscapeButton(noBtn);
	mb->setDefaultButton(noBtn);

	return mb->exec() == QMessageBox::Yes;
}

void standardErrorDialog(const QString message, QWidget *parent)
{
	execOnMainThread([message, parent]{
		QMessageBox *mb = new QMessageBox( QMessageBox::Critical, QObject::tr("Chyba"), message );
		mb->setParent( parent );
		mb->setWindowModality( Qt::ApplicationModal );
		mb->addButton( QObject::tr("Ok"), QMessageBox::AcceptRole );
		mb->show();
	});
}

bool standardConfirmDialog(const QString message, QWidget *parent)
{
	QMessageBox *mb = new QMessageBox( QMessageBox::Question, QObject::tr("Potvrzení"), message );
	mb->setParent( parent );
	mb->setWindowModality( Qt::ApplicationModal );

	mb->addButton( QObject::tr("Ano"), QMessageBox::YesRole );
	auto noBtn = mb->addButton( QObject::tr("Ne"), QMessageBox::NoRole );

	mb->setEscapeButton(noBtn);
	mb->setDefaultButton(noBtn);

	return mb->exec() == QMessageBox::Yes;
}
