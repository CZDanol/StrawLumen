#include "regex.h"

void replaceCallback(QString &str, const QRegularExpression &regex, const std::function<QString (const QRegularExpressionMatch &)> &callback)
{
	int correction = 0;
	auto it = regex.globalMatch(str);
	while(it.hasNext()) {
		auto m = it.next();
		QString replacement = callback(m);
		str.replace(m.capturedStart() + correction, m.capturedLength(), replacement);
		correction += replacement.length() - m.capturedLength();
	}
}
