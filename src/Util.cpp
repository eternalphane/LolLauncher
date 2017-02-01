#include "Util.hpp"
#include "Error.hpp"
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>

std::pair<DWORD, PROCESS_INFORMATION> Util::createProcess(
    const std::string &exec,
    const std::string &args,
    const std::string &dir)
{
    auto old_env = GetEnvironmentStrings();
    std::string env("__COMPAT_LAYER=ElevateCreateProcess");
    env += (char)0;
    for (auto p = (char *)old_env; *p; p += lstrlen(p) + 1)
        env = env + p + (char)0;
    env += (char)0;
    FreeEnvironmentStrings(old_env);
    STARTUPINFOA si;
    SecureZeroMemory(&si, sizeof(STARTUPINFOA));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    SecureZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    DWORD err;
    if (!CreateProcess(
            exec.c_str(),
            (LPSTR)args.c_str(),
            NULL,
            NULL,
            FALSE,
            0,
            (LPVOID)env.c_str(),
            dir.c_str(),
            &si,
            &pi))
        err = GetLastError();
    else
        err = 0;
    return {err, pi};
}

std::string Util::getErrMsg(DWORD dw)
{
    LPSTR tmp;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&tmp,
        0,
        NULL);
    std::unique_ptr<char, HLOCAL(__stdcall *)(HLOCAL)> msg_buf(tmp, &LocalFree);
    *(msg_buf.get() + size - 2) = 0;
    return std::string(msg_buf.get());
}

nlohmann::json Util::getConf(const std::string &path)
{
    std::ifstream f(path);
    if (!f.is_open())
        throw Error(path + " not found!");
    nlohmann::json j;
    f >> j;
    return j;
}

std::string Util::getHex(unsigned byte)
{
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setw(2) << std::setfill('0') << byte;
    return oss.str();
}

std::string Util::getHex(const std::vector<char> &data)
{
    std::ostringstream oss;
    oss << std::hex;
    for (auto byte : data)
        oss << std::setw(2) << std::setfill('0') << (int)(unsigned char)byte << ' ';
    return oss.str();
}
