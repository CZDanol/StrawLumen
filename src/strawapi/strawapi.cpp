#include "strawapi.h"

#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>

const QUrl StrawApi::apiUrl("https://api.straw-solutions.cz/");

StrawApi::RequestResult StrawApi::requestJson(const QJsonObject &request, QJsonObject &response)
{
	QNetworkAccessManager mgr;
	QEventLoop evLoop;

	QObject::connect(&mgr,SIGNAL(finished(QNetworkReply*)),&evLoop,SLOT(quit()));

	QNetworkRequest req(apiUrl);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	req.setHeader(QNetworkRequest::UserAgentHeader, "StrawSolutionsAPI/1.0");

	QScopedPointer<QNetworkReply> reply(mgr.post(req, QJsonDocument(request).toJson(QJsonDocument::Compact)));

	// Wait for the response
	evLoop.exec();

	if(reply->error() != QNetworkReply::NoError) {
		qWarning() << "(api request) HTTP error: " << reply->errorString();
		return RequestResult::connectionProblem;
	}

	const QByteArray rawResponse = reply->readAll();

	QJsonParseError jsonErr;
	response = QJsonDocument::fromJson(rawResponse, &jsonErr).object();

	if(jsonErr.error != QJsonParseError::NoError) {
		qWarning() << "(api request) JSON error: " << jsonErr.errorString();
		qDebug() << "receivedData: " << QString::fromUtf8(rawResponse);

		return RequestResult::wrongResponse;
	}

	return RequestResult::ok;
}
