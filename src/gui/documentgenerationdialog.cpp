#include "documentgenerationdialog.h"
#include "ui_documentgenerationdialog.h"

#include "job/db.h"

#include <QLayout>
#include <QCoreApplication>
#include <QDir>
#include <QWebEnginePage>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>

DocumentGenerationDialog *documentGenerationDialog = nullptr;

DocumentGenerationDialog::DocumentGenerationDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DocumentGenerationDialog)
{
	ui->setupUi(this);

	connect(&view_, &QWebEngineView::loadFinished, this, &DocumentGenerationDialog::onLoaded);
	layout()->addWidget(&view_);
}

DocumentGenerationDialog::~DocumentGenerationDialog()
{
	delete ui;
}

void DocumentGenerationDialog::onLoaded()
{
	view_.page()->runJavaScript(jqueryJs);
	view_.page()->runJavaScript(genJs);
}

void DocumentGenerationDialog::on_pushButton_clicked()
{
	view_.load(QUrl::fromLocalFile(QDir(qApp->applicationDirPath()).absoluteFilePath("../documenttemplates/test.html")));

	QFile jqueryFile(QDir(qApp->applicationDirPath()).absoluteFilePath("../documenttemplates/jquery.js"));
	jqueryFile.open(QIODevice::ReadOnly);

	jqueryJs = QString::fromUtf8(jqueryFile.readAll());
	//QWebEngineScript jqueryScript;
	//jqueryScript.setSourceCode( QString::fromUtf8(jqueryFile.readAll()));
	//jqueryScript.setWorldId(QWebEngineScript::UserWorld);
	//view_.page()->scripts().insert(jqueryScript);

	QJsonObject json;
	{
		QJsonArray jsonSongs;

		QSqlQuery q = db->selectQuery("SELECT * FROM songs ORDER BY name ASC");
		while(q.next()) {
			QJsonObject jsonSong;

			jsonSong["name"] = q.value("name").toString();
			jsonSong["author"] = q.value("author").toString();
			jsonSong["content"] = q.value("content").toString();

			jsonSongs.append(jsonSong);
		}

		json["songs"] = jsonSongs;
	}

	//QWebEngineScript genScript;
	//genScript.setSourceCode(QString("$(document).ready(function(){generateDocument(%1);});").arg(QString::fromUtf8(QJsonDocument(json).toJson(QJsonDocument::Compact))));
	/*genScript.setWorldId(QWebEngineScript::UserWorld);
	view_.page()->scripts().insert(genScript);*/
	//view_.page()->runJavaScript(QString("generateDocument(%1);").arg(QString::fromUtf8(QJsonDocument(json).toJson(QJsonDocument::Compact))));
	genJs = QString("generateDocument(%1);").arg(QString::fromUtf8(QJsonDocument(json).toJson(QJsonDocument::Compact)));
}

void DocumentGenerationDialog::on_pushButton_2_clicked()
{
	view_.page()->printToPdf(QString("test.pdf"));
}
