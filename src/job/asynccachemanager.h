#ifndef ASYNCCACHEMANAGER_H
#define ASYNCCACHEMANAGER_H

#include <functional>

#include <QCache>
#include <QObject>
#include <QVariant>

class AsyncCacheManagerRecord : public QObject {
	Q_OBJECT

public:
	QVariant data;
};

class AsyncCacheManager : public QObject {
	Q_OBJECT

public:
	using CheckStornoFunction = std::function<bool()>;

	/// Arguments: ref result, ref cost (memory usage), storno function; returning: false on interrupted
	using ProduceFunction = std::function<bool(QVariant &, int &, const CheckStornoFunction &)>;
	using CallbackFunction = std::function<void()>;

public:
	AsyncCacheManager();

public:
	QVariant request(const QString key, ProduceFunction produceFunction, QObject *callbackParent = nullptr, const CallbackFunction &callback = nullptr);

private:
	QCache<QString, AsyncCacheManagerRecord> cache_;
};

extern AsyncCacheManager *asyncCache;

#endif// ASYNCCACHEMANAGER_H
