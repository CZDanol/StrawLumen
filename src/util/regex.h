#ifndef REGEX_H
#define REGEX_H

#include <functional>

#include <QRegularExpression>

void replaceCallback(QString &str, const QRegularExpression &regex, const std::function<QString(const QRegularExpressionMatch &m)> &callback);

#endif// REGEX_H
