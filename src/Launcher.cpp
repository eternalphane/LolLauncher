#include "Launcher.hpp"
#include "Error.hpp"
#include "Logger.hpp"
#include "Util.hpp"
#include <chrono>
#include <fstream>

Launcher::Launcher(
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
    const std::string &get_client_ip_url) : clientPort(client_port),
                                            gamePort(game_port),
                                            lolRootPath(lol_root_path),
                                            gameSignatureLength(game_signature_length),
                                            szGameSignature(sz_game_signature),
                                            cltkeyLength(cltkey_length),
                                            cltkey(cltkey),
                                            uId(u_id),
                                            host(host),
                                            xmppServerUrl(xmpp_server_url),
                                            lqUri(lq_uri),
                                            getClientIpUrl(get_client_ip_url),
                                            srvLolClient(clientPort, makeClientProtocol()),
                                            srvLolGame(gamePort, makeGameProtocol()) {}

Launcher::~Launcher() { finish(); }

Protocol Launcher::makeClientProtocol()
{
    using raw_bytes = Protocol::raw_bytes;
    using message = Protocol::message;
    using Listener = Protocol::Listener;
    using namespace std::chrono_literals;
    return Protocol(
        Listener{
            [this]() {
                Logger::log(LOG_STDOUT, "Client connected.");
                std::thread(
                    [this]() {
                        try
                        {
                            while (srvLolClient.status() > 0)
                            {
                                std::this_thread::sleep_for(Protocol::HEARTBEAT_TIME_INTERVAL * 1s);
                                srvLolClient.send(Protocol::makePacket(
                                    Protocol::MAESTROMESSAGETYPE_HEARTBEAT,
                                    raw_bytes()));
                            }
                        }
                        catch (...)
                        {
                        }
                    })
                    .detach();
            },
            [this](message &&msg) {
                unsigned command;
                raw_bytes body;
                std::tie(command, body) = msg;
                Logger::log(LOG_FILE, "ReceiveMessage [", command, "]");
                switch (command)
                {
                case Protocol::MAESTROMESSAGETYPE_GAMECLIENT_CREATE:
                    Logger::log(LOG_STDOUT, "Login key: ", body.data());
                    launchGame(body.data());
                    return Protocol::makeMsg(Protocol::MAESTROMESSAGETYPE_REPLY, raw_bytes());
                case Protocol::MAESTROMESSAGETYPE_CLOSE:
                    break;
                case Protocol::MAESTROMESSAGETYPE_HEARTBEAT:
                    return Protocol::makeMsg(Protocol::MAESTROMESSAGETYPE_REPLY, raw_bytes());
                case Protocol::MAESTROMESSAGETYPE_REPLY:
                    break;
                case Protocol::MAESTROMESSAGETYPE_CHATMESSAGE_TO_GAME:
                    srvLolGame.send(Protocol::makePacket(command, std::forward<message>(msg).second));
                    break;
                default:
                    Logger::log(LOG_STDOUT, "Client: Ignored message type ", command);
                }
                return Protocol::makeMsg(Protocol::MAESTROMESSAGETYPE_IGNORE, raw_bytes());
            },
            []() {
                Logger::log(LOG_STDOUT, "Client disconnected.");
            }});
}

Protocol Launcher::makeGameProtocol()
{
    using raw_bytes = Protocol::raw_bytes;
    using message = Protocol::message;
    using Listener = Protocol::Listener;
    using namespace std::chrono_literals;
    return Protocol(
        Listener{
            [this]() {
                Logger::log(LOG_STDOUT, "Game connected.");
                std::thread(
                    [this]() {
                        try
                        {
                            while (srvLolGame.status() > 0)
                            {
                                std::this_thread::sleep_for(Protocol::HEARTBEAT_TIME_INTERVAL * 1s);
                                srvLolGame.send(Protocol::makePacket(
                                    Protocol::MAESTROMESSAGETYPE_HEARTBEAT,
                                    raw_bytes()));
                            }
                        }
                        catch (...)
                        {
                        }
                    })
                    .detach();
            },
            [this](message &&msg) {
                unsigned command;
                raw_bytes body;
                std::tie(command, body) = msg;
                Logger::log(LOG_FILE, "ReceiveMessage [", command, "]");
                switch (command)
                {
                case Protocol::MAESTROMESSAGETYPE_GAMECLIENT_STOPPED:
                    break;
                case Protocol::MAESTROMESSAGETYPE_CLOSE:
                    return Protocol::makeMsg(Protocol::MAESTROMESSAGETYPE_IGNORE, raw_bytes());
                case Protocol::MAESTROMESSAGETYPE_HEARTBEAT:
                    break;
                case Protocol::MAESTROMESSAGETYPE_REPLY:
                    return Protocol::makeMsg(Protocol::MAESTROMESSAGETYPE_IGNORE, raw_bytes());
                case Protocol::MAESTROMESSAGETYPE_GAMECLIENT_ABANDONED:
                    srvLolClient.send(Protocol::makePacket(command, raw_bytes()));
                    break;
                case Protocol::MAESTROMESSAGETYPE_GAMECLIENT_LAUNCHED:
                    srvLolClient.send(Protocol::makePacket(command, raw_bytes()));
                    break;
                case Protocol::MAESTROMESSAGETYPE_GAMECLIENT_CONNECTED_TO_SERVER:
                    srvLolClient.send(Protocol::makePacket(command, raw_bytes()));
                    break;
                case Protocol::MAESTROMESSAGETYPE_CHATMESSAGE_FROM_GAME:
                    srvLolClient.send(Protocol::makePacket(command, std::forward<message>(msg).second));
                    break;
                default:
                    Logger::log(LOG_STDOUT, "Game: Ignored message type ", command);
                    return Protocol::makeMsg(Protocol::MAESTROMESSAGETYPE_IGNORE, raw_bytes());
                }
                return Protocol::makeMsg(Protocol::MAESTROMESSAGETYPE_REPLY, raw_bytes());
            },
            []() {
                Logger::log(LOG_STDOUT, "Game disconnected.");
            }});
}

void Launcher::launchClient()
{
    Logger::log(LOG_STDOUT, "Client start...");
    Logger::log(LOG_FILE, "Client start...");
    auto proc_info = Util::createProcess(
        lolRootPath + "\\Air\\LolClient.exe",
        "Air\\LolClient.exe -runtime .\\ -nodebug META-INF\\AIR\\application.xml .\\ --" + clientPort +
            " gameSignatureLength=" + gameSignatureLength +
            " szGameSignature=" + szGameSignature +
            " cltkeyLength=" + cltkeyLength +
            " cltkey=" + cltkey +
            " uId=" + uId +
            " --host=" + host +
            " --xmpp_server_url=" + xmppServerUrl +
            " --lq_uri=" + lqUri +
            " --getClientIpURL=" + getClientIpUrl,
        lolRootPath);
    if (proc_info.first)
        throw Error(Util::getErrMsg(proc_info.first));
}

void Launcher::launchGame(const char *args)
{
    Logger::log(LOG_STDOUT, "Game start...");
    Logger::log(LOG_FILE, "Game start...");
    auto proc_info = Util::createProcess(
        lolRootPath + "\\Game\\League of Legends.exe",
        "Game\\League of Legends.exe " + gamePort + " lol.launcher_tencent.exe Air\\LolClient.exe " + args,
        lolRootPath);
    if (proc_info.first)
        throw Error(Util::getErrMsg(proc_info.first));
}

void Launcher::launch()
{
    Logger::create(LOG_STDOUT, std::cout);
    Logger::create(LOG_FILE, std::ofstream("LolLauncher.log", std::ios::app));
    try
    {
        srvLolClient.start();
        Logger::log(LOG_STDOUT, "Listening client...");
        srvLolGame.start();
        Logger::log(LOG_STDOUT, "Listening game...");
        launchClient();
    }
    catch (Error &e)
    {
        Logger::log(LOG_STDOUT, e.what());
    }
}

void Launcher::finish()
{
    for (auto &eptr : srvLolClient.stop())
    {
        try
        {
            std::rethrow_exception(eptr);
        }
        catch (Error &e)
        {
            Logger::log(LOG_STDOUT, e.what());
        }
    }
    for (auto &eptr : srvLolGame.stop())
    {
        try
        {
            std::rethrow_exception(eptr);
        }
        catch (Error &e)
        {
            Logger::log(LOG_STDOUT, e.what());
        }
    }
}
