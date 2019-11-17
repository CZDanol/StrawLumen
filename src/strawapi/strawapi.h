#ifndef STRAWAPI_H
#define STRAWAPI_H

#include <QJsonObject>
#include <QUrl>

class StrawApi
{

public:
	enum class RequestResult : int {
		ok,
		connectionProblem,
		wrongResponse,
		__count
	};

public:
	static RequestResult requestJson(const QJsonObject &jsonRequest, QJsonObject &response);

};

#endif // STRAWAPI_H
