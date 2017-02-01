#pragma once
#include <functional>
#include <vector>

class Protocol
{
  public:
    using raw_bytes = std::vector<char>;
    using message = std::pair<unsigned, raw_bytes>;
    struct Listener
    {
        std::function<void()> onConnect;
        std::function<message(message &&)> onMsgRecv;
        std::function<void()> onClose;
    };

    static const unsigned HEARTBEAT_TIME_INTERVAL = 25;
    static const unsigned PACKET_HEADER_SIZE = 16;
    static const unsigned MAESTROMESSAGETYPE_IGNORE = 0xff;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_CREATE = 0x00;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_STOPPED = 0x01;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_CRASHED = 0x02;
    static const unsigned MAESTROMESSAGETYPE_CLOSE = 0x03;
    static const unsigned MAESTROMESSAGETYPE_HEARTBEAT = 0x04;
    static const unsigned MAESTROMESSAGETYPE_REPLY = 0x05;
    static const unsigned MAESTROMESSAGETYPE_LAUNCHERCLIENT = 0x06;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_ABANDONED = 0x07;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_LAUNCHED = 0x08;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_VERSION_MISMATCH = 0x09;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_CONNECTED_TO_SERVER = 0x0a;
    static const unsigned MAESTROMESSAGETYPE_CHATMESSAGE_TO_GAME = 0x0b;
    static const unsigned MAESTROMESSAGETYPE_CHATMESSAGE_FROM_GAME = 0x0c;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_CREATE_VERSION = 0x0d;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_INSTALL_VERSION = 0x0e;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_CANCEL_INSTALL = 0x0f;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_INSTALL_PROGRESS = 0x10;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_INSTALL_PREVIEW = 0x11;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_CANCEL_PREVIEW = 0x12;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_PREVIEW_PROGRESS = 0x13;
    static const unsigned MAESTROMESSAGETYPE_PLAY_REPLAY = 0x14;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_UNINSTALL_VERSION = 0x15;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_CANCEL_UNINSTALL = 0x16;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_UNINSTALL_PROGRESS = 0x17;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_UNINSTALL_PREVIEW = 0x18;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_CANCEL_UNINSTALL_PREVIEW = 0x19;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_PREVIEW_UNINSTALL_PROGRESS = 0x1a;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_ENUMERATE_VERSIONS = 0x1b;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_CREATE_CLIENT_AND_PRELOAD = 0x1c;
    static const unsigned MAESTROMESSAGETYPE_GAMECLIENT_START_PRELOADED_GAME = 0x1d;

  private:
    struct Packet
    {
        struct Header
        {
            unsigned head0;
            unsigned head1;
            unsigned command;
            unsigned body_len;
        } header{0x10, 0x01, 0xff, 0x00};
        raw_bytes body;

        bool complete();
    } packet;
    Listener listener;

    bool makePacket(message &&msg);

  public:
    Protocol(Listener &&listener);
    static message makeMsg(unsigned command, const raw_bytes &body);
    static message makeMsg(unsigned command, raw_bytes &&body);
    static raw_bytes makePacket(unsigned command, raw_bytes &&body);
    void onConnect();
    bool onRecv(const raw_bytes &data);
    bool onHandle();
    raw_bytes &onSend();
    void onClose();
};
