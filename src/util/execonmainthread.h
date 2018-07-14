#ifndef EXECONMAINTHREAD_H
#define EXECONMAINTHREAD_H

#include <functional>

void execOnMainThread(const std::function<void()> &job);

#endif // EXECONMAINTHREAD_H
