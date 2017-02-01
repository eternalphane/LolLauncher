#pragma once
#pragma warning(disable : 4996)
#include "Error.hpp"
#include <ctime>
#include <iomanip>
#include <utility>

template <typename T, typename... Args>
void Logger::Log::write(const T &msg, Args &&... msgs)
{
    *os << msg;
    write(std::forward<Args>(msgs)...);
}

template <typename... Args>
void Logger::Log::writeln(Args &&... msgs)
{
    std::lock_guard<std::mutex> lock(logMutex);
    if (!os->rdbuf())
    {
        throw Error("Log stream not ready.");
        return;
    }
    std::time_t t = std::time(nullptr);
    *os << std::put_time(std::localtime(&t), "[%Y/%m/%d %H:%M:%S] ");
    write(std::forward<Args>(msgs)...);
    *os << std::endl;
}

template <typename... Args>
void Logger::log(int id, Args &&... msgs)
{
    auto it = logs.find(id);
    if (it != logs.end())
        it->second->writeln(std::forward<Args>(msgs)...);
    else
        throw Error("Log id not found.");
}
