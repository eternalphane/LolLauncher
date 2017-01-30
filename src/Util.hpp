#pragma once
#include "external/json.hpp"
#include <string>
#include <windows.h>

class Util
{
  private:
    Util() {}

  public:
    static std::pair<DWORD, PROCESS_INFORMATION> createProcess(
        const std::string &exec,
        const std::string &args,
        const std::string &dir);
    static std::string getErrMsg(DWORD dw);
    static nlohmann::json getConf(const std::string &path);
};
