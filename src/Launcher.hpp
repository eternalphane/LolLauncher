#pragma once
#include "Protocol.hpp"
#include "TcpServer.hpp"
#include <exception>
#include <string>
#include <strsafe.h>
#include <thread>
#include <windows.h>

class Launcher
{
  private:
    static const int LOG_FILE = 0;
    static const int LOG_STDOUT = 1;
    std::string clientPort;
    std::string gamePort;
    TcpServer srvLolClient;
    TcpServer srvLolGame;
    std::string lolRootPath;
    std::string gameSignatureLength;
    std::string szGameSignature;
    std::string cltkeyLength;
    std::string cltkey;
    std::string uId;
    std::string host;
    std::string xmppServerUrl;
    std::string lqUri;
    std::string getClientIpUrl;

    Protocol makeClientProtocol();
    Protocol makeGameProtocol();
    void launchClient();
    void launchGame(const char *args);

  public:
    Launcher(
        const std::string &client_port,
        const std::string &game_port,
        const std::string &lol_root_path,
        const std::string &game_signature_length,
        const std::string &sz_game_signature,
        const std::string &cltkey_length,
        const std::string &cltkey,
        const std::string &u_id,
        const std::string &host,
        const std::string &xmpp_server_url,
        const std::string &lq_uri,
        const std::string &get_client_ip_url);
    ~Launcher();
    void launch();
    void finish();
};
