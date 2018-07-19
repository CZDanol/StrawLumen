#ifndef STRAWAPI_H
#define STRAWAPI_H

#include <QJsonObject>
#include <QUrl>

class StrawApi
{

public:
	enum class RequestResult {
		ok,
		connectionProblem,
		wrongResponse
	};

public:
	static RequestResult requestJson(const QJsonObject &jsonRequest, QJsonObject &response);

public:
	static const QUrl apiUrl;

};

#endif // STRAWAPI_H
