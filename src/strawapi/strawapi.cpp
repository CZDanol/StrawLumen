#include "strawapi.h"

#include <QEventLoop>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

StrawApi::RequestResult StrawApi::requestJson(const QJsonObject &requestJson, QJsonObject &response) {
	QNetworkAccessManager mgr;
	QEventLoop evLoop;

	QObject::connect(&mgr, SIGNAL(finished(QNetworkReply *)), &evLoop, SLOT(quit()));

	QUrl url(QString::fromUtf8(QByteArray::fromBase64(QByteArray("aHR0cHM6Ly9hcGkyLnN0cmF3LXNvbHV0aW9ucy5jei8="))));
	QNetworkRequest req(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	req.setHeader(QNetworkRequest::UserAgentHeader, "StrawAPI/1.0");

	const QByteArray requestData = QJsonDocument(requestJson).toJson(QJsonDocument::Compact);
	QScopedPointer<QNetworkReply> reply(mgr.post(req, requestData));

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
		qWarning() << "(api request) JSON error: " << jsonErr.errorString() << rawResponse;
		return RequestResult::wrongResponse;
	}

	return RequestResult::ok;
}
