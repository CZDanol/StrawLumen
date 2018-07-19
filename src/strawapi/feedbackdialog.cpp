#include "feedbackdialog.h"
#include "ui_feedbackdialog.h"

#include <QShortcut>

#include "strawapi/strawapi.h"
#include "gui/mainwindow.h"
#include "util/standarddialogs.h"
#include "gui/splashscreen.h"

FeedbackDialog::FeedbackDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FeedbackDialog)
{
	ui->setupUi(this);

	new QShortcut(Qt::CTRL + Qt::Key_Return, ui->btnSend, SLOT(click()));
}

FeedbackDialog::~FeedbackDialog()
{
	delete ui;
}

void FeedbackDialog::show()
{
	ui->teMessage->clear();
	QDialog::show();
}

void FeedbackDialog::on_btnCancel_clicked()
{
	reject();
}

FeedbackDialog *feedbackDialog()
{
	static FeedbackDialog *dlg = nullptr;
	if(!dlg)
		dlg = new FeedbackDialog(mainWindow);

	return dlg;
}

void FeedbackDialog::on_btnSend_clicked()
{
	const QString email = ui->lnEmail->text().trimmed();
	if(email.isEmpty())
		return standardErrorDialog(tr("Prosím vyplňte kontaktní e-mail"));

	const QString message = ui->teMessage->toPlainText().trimmed();
	if(message.isEmpty())
		return standardErrorDialog(tr("Prosím vyplňte tělo zprávy."));

	const QJsonObject request {
		{"action", "sendFeedback"},
		{"product", PRODUCT_IDSTR},
		{"contact", email},
		{"message", message}
	};

	StrawApi::RequestResult result;
	QJsonObject response;

	splashscreen->asyncAction(tr("Odesílání zprávy"), false, [&]{
		result = StrawApi::requestJson(request, response);
	});

	if(result != StrawApi::RequestResult::ok || response["status"] != "ok")
		return standardErrorDialog(tr("Je nám líto, ale zprávu se nepodařilo odeslat."));
	else
		standardSuccessDialog(tr("Zpětná vazba byla odeslána. Děkujeme!"));

	accept();
}
