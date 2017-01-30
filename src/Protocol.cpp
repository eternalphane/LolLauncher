#pragma warning(disable : 4996)
#include "Protocol.hpp"
#include <algorithm>
#include <intrin.h>

using raw_bytes = Protocol::raw_bytes;
using message = Protocol::message;

Protocol::Protocol(Listener &&listener) : listener(std::move(listener)) {}

bool Protocol::Packet::complete() { return body.size() != 0 && body.size() == header.body_len; }

void Protocol::headerSwapEndian(Protocol::Packet::Header &header)
{
    header.head0 = _byteswap_ulong(header.head0);
    header.head1 = _byteswap_ulong(header.head1);
    header.command = _byteswap_ulong(header.command);
    header.body_len = _byteswap_ulong(header.body_len);
}

bool Protocol::makePacket(message &&msg)
{
    packet.header.head0 = 0x10;
    packet.header.head1 = 0x01;
    packet.header.command = msg.first;
    packet.body = std::move(msg.second);
    packet.header.body_len = packet.body.size();
    return msg.first == MAESTROMESSAGETYPE_IGNORE;
}

message Protocol::makeMsg(unsigned command, const raw_bytes &body)
{
    return std::make_pair(command, body);
}

message Protocol::makeMsg(unsigned command, raw_bytes &&body)
{
    return std::make_pair(command, std::forward<raw_bytes>(body));
}

raw_bytes Protocol::makePacket(unsigned command, raw_bytes &&body)
{
    Packet::Header h{0x10, 0x01, command, body.size()};
    headerSwapEndian(h);
    body.insert(body.begin(), (char *)&h, (char *)&h + PACKET_HEADER_SIZE);
    return body;
}

void Protocol::onConnect() { listener.onConnect(); }

bool Protocol::onRecv(const raw_bytes &data)
{
    packet.body.reserve(packet.body.size() + data.size());
    auto it_f = data.data(), it_l = it_f + data.size();
    if (!packet.body.size() && !packet.header.body_len)
    {
        std::copy(it_f, it_f + PACKET_HEADER_SIZE, (char *)&packet.header);
        it_f += PACKET_HEADER_SIZE;
    }
    packet.body.insert(packet.body.end(), it_f, it_l);
    return packet.complete();
}

bool Protocol::onHandle()
{
    headerSwapEndian(packet.header);
    return makePacket(listener.onMsgRecv(makeMsg(packet.header.command, packet.body)));
}

raw_bytes &Protocol::onSend()
{
    headerSwapEndian(packet.header);
    packet.body.insert(packet.body.begin(), (char *)&packet.header, (char *)&packet.header + PACKET_HEADER_SIZE);
    packet.header.body_len = 0;
    return packet.body;
}

void Protocol::onClose() { listener.onClose(); }
