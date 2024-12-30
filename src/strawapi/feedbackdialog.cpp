#include "feedbackdialog.h"
#include "ui_feedbackdialog.h"

#include <QDesktopServices>
#include <QShortcut>
#include <QUrl>

#include "gui/mainwindow.h"
#include "gui/splashscreen.h"
#include "strawapi/strawapi.h"
#include "util/standarddialogs.h"

FeedbackDialog::FeedbackDialog(QWidget *parent) : QDialog(parent),
																									ui(new Ui::FeedbackDialog) {
	ui->setupUi(this);

	new QShortcut(Qt::CTRL | Qt::Key_Return, ui->btnSend, SLOT(click()));
}

FeedbackDialog::~FeedbackDialog() {
	delete ui;
}

void FeedbackDialog::show() {
	ui->teMessage->clear();
	ui->lnEmail->setFocus();
	QDialog::show();
}

void FeedbackDialog::on_btnCancel_clicked() {
	reject();
}

FeedbackDialog *feedbackDialog() {
	static FeedbackDialog *dlg = nullptr;
	if(!dlg)
		dlg = new FeedbackDialog(mainWindow);

	return dlg;
}

void FeedbackDialog::on_btnSend_clicked() {
	const QString email = ui->lnEmail->text().trimmed();
	if(email.isEmpty())
		return standardErrorDialog(tr("Prosím vyplňte kontaktní e-mail"));

	const QString message = ui->teMessage->toPlainText().trimmed();
	if(message.isEmpty())
		return standardErrorDialog(tr("Prosím vyplňte tělo zprávy."));

	const QString subject = ui->lnSubject->text().trimmed();

	const QJsonObject request{
		{"action", "feedback"},
		{"product", PRODUCT_IDSTR},
		{"subject", subject},
		{"message", QStringLiteral("Kontakt: %1\n\n%2").arg(email, message)}};

	StrawApi::RequestResult result;
	QJsonObject response;

	splashscreen->asyncAction(tr("Odesílání zprávy"), false, [&] {
		result = StrawApi::requestJson(request, response);
	});

	if(result != StrawApi::RequestResult::ok || response["status"] != "ok")
		return standardErrorDialog(tr("Je nám líto, ale zprávu se nepodařilo odeslat."));
	else
		standardSuccessDialog(tr("Zpětná vazba byla odeslána. Děkujeme!"));

	accept();
}

void FeedbackDialog::on_btnFacebook_clicked() {
	QDesktopServices::openUrl(QUrl("https://www.facebook.com/strawLumen"));
}
