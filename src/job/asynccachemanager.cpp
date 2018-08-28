#include "asynccachemanager.h"

#include <QPointer>
#include <QtConcurrent/QtConcurrent>

#include "util/execonmainthread.h"

AsyncCacheManager *asyncCache = nullptr;

AsyncCacheManager::AsyncCacheManager()
{
	cache_.setMaxCost(200 * 1024 * 1024);
}

QVariant AsyncCacheManager::request(const QString key, AsyncCacheManager::ProduceFunction produceFunction, QObject *callbackParent, const AsyncCacheManager::CallbackFunction &callback)
{
	AsyncCacheManagerRecord *rec = cache_[key];

	if(!rec) {
		rec = new AsyncCacheManagerRecord();
		cache_.insert(key, rec);

		QPointer<AsyncCacheManagerRecord> recPtr(rec);
		QPointer<QObject> callbackParentPtr(callbackParent);

		QtConcurrent::run([=]{
			CheckStornoFunction chsf = [recPtr]{
				return recPtr.isNull();
			};

			QVariant data;
			int cost = 1;
			bool result = false;

			if(!chsf())
				result = produceFunction(data, cost, chsf);

			execOnMainThread([=]{
				if(recPtr.isNull())
					return;

				if(!result || (callbackParent && callbackParentPtr.isNull()))
					cache_.remove(key);

				else {
					recPtr->data = data;

					cache_.take(key);
					cache_.insert(key, recPtr.data(), cost);

					if(callback)
						callback();
				}
			});
		});

		return QVariant();
	}

	return rec->data;
}
