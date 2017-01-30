#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_CLIENT_PORT "8393"
#define DEFAULT_GAME_PORT "8394"
#include "Launcher.hpp"
#include "Logger.hpp"
#include "Util.hpp"
#include <csignal>
#include <fstream>
#include <iostream>

bool flag = true;

int main()
{
    auto conf = Util::getConf("conf.json");
    Launcher launcher(
        DEFAULT_CLIENT_PORT,
        DEFAULT_GAME_PORT,
        conf["lolRootPath"],
        conf["gameSignatureLength"],
        conf["szGameSignature"],
        conf["cltkeyLength"],
        conf["cltkey"],
        conf["uId"],
        conf["host"],
        conf["xmppServerUrl"],
        conf["lqUri"],
        conf["getClientIpUrl"]);
    launcher.launch();
    signal(SIGINT, [](int sig) { flag = false; });
    while (flag)
    {
    }
    launcher.finish();
    return 0;
}
