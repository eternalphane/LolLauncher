#include "Logger.hpp"

Logger::LogMap Logger::logs;

Logger::Log::Log(const std::ostream &os) : os(os.rdbuf()) {}

Logger::Log::Log(std::ostream &&os) : os(os.rdbuf()) {}

void Logger::Log::write() {}

bool Logger::Log::init(std::ostream &os)
{
    std::lock_guard<std::mutex> lock(logMutex);
    if (!this->os.rdbuf())
        this->os.rdbuf(os.rdbuf());
    return this->os.rdbuf();
}

bool Logger::create(int id, const std::ostream &os)
{
    return logs.insert(std::make_pair(id, std::make_unique<Log>(os))).second;
}

bool Logger::create(int id, std::ostream &&os)
{
    return logs.insert(std::make_pair(id, std::make_unique<Log>(std::forward<std::ostream>(os)))).second;
}
