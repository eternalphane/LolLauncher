#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <ostream>

class Logger
{
  private:
    class Log
    {
      private:
        std::unique_ptr<std::ostream> os;
        std::mutex logMutex;

        void write();
        template <typename T, typename... Args>
        void write(const T &msg, Args &&... msgs);

      public:
        Log(const std::ostream &os);
        Log(std::ostream *os);
        bool init(const std::ostream &os);
        bool init(std::ostream *os);
        template <typename... Args>
        void writeln(Args &&... msgs);
    };

    using LogMap = std::map<int, std::unique_ptr<Log>>;

    static LogMap logs;

    Logger() {}

  public:
    static bool create(int id, const std::ostream &os);
    static bool create(int id, std::ostream *os);
    template <typename... Args>
    static void log(int id, Args &&... msgs);
};

#include "Logger.tcc"
