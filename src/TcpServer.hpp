#pragma once
#include "Protocol.hpp"
#include <atomic>
#include <exception>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

class TcpServer
{
    using raw_bytes = Protocol::raw_bytes;

  private:
    static const std::chrono::milliseconds SLEEP_TIME;
    std::string port;
    Protocol protocol;
    std::thread tComm;
    std::queue<raw_bytes> qSend;
    std::mutex mutexSend;
    std::exception_ptr eComm;
    std::exception_ptr eRecv;
    std::exception_ptr eSend;
    std::atomic_int activate = -1;

    static void cleanup(SOCKET socket = INVALID_SOCKET);
    static std::string getErrMsg(const char *tag, int err_no);
    SOCKET bind();
    void communicate(SOCKET socket);
    void recvLoop(SOCKET socket);
    void sendLoop(SOCKET socket);

  public:
    using eptr_list = std::vector<std::exception_ptr>;
    TcpServer(const std::string &port, Protocol &&protocol);
    ~TcpServer();
    int status();
    void start();
    void send(raw_bytes &&data);
    eptr_list stop();
};
