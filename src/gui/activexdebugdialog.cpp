#include "activexdebugdialog.h"
#include "ui_activexdebugdialog.h"

#include "gui/mainwindow.h"
#include "util/execonmainthread.h"

ActiveXDebugDialog::ActiveXDebugDialog(QWidget *parent) : QDialog(parent),
                                                          ui(new Ui::ActiveXDebugDialog) {
	ui->setupUi(this);
}

ActiveXDebugDialog::~ActiveXDebugDialog() {
	delete ui;
}

void ActiveXDebugDialog::show(QString html) {
	execOnMainThread([=] {
		ui->tb->setHtml(html);
		QDialog::show();
	});
}

ActiveXDebugDialog *activeXDebugDialog() {
	static ActiveXDebugDialog *dlg = nullptr;
	if(!dlg)
		dlg = new ActiveXDebugDialog(mainWindow);

	return dlg;
}
