/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** NetworkMessage
*/

#include "NetworkMessage.hpp"

#include <cstring>
#include <sstream>

namespace rtype {

::LoginPacket NetworkMessage::createLoginPacket(const std::string& username,
                                                uint32_t sequenceId)
{
    ::LoginPacket packet = {};
    packet.header.opCode = OpCode::C2S_LOGIN;
    packet.header.packetSize = sizeof(LoginPacket);
    packet.header.sequenceId = sequenceId;

    size_t copyLen = std::min(username.length(), sizeof(packet.username) - 1);
    std::memcpy(packet.username, username.c_str(), copyLen);
    packet.username[copyLen] = '\0';

    return packet;
}

::InputPacket NetworkMessage::createInputPacket(uint8_t inputMask,
                                                uint32_t sequenceId)
{
    ::InputPacket packet = {};
    packet.header.opCode = OpCode::C2S_INPUT;
    packet.header.packetSize = sizeof(InputPacket);
    packet.header.sequenceId = sequenceId;
    packet.inputMask = inputMask;
    return packet;
}

::Header NetworkMessage::createDisconnectPacket(uint32_t sequenceId)
{
    ::Header packet = {};
    packet.opCode = OpCode::C2S_DISCONNECT;
    packet.packetSize = sizeof(Header);
    packet.sequenceId = sequenceId;
    return packet;
}

::Header NetworkMessage::createAckPacket(uint32_t sequenceId)
{
    ::Header packet = {};
    packet.opCode = OpCode::C2S_ACK;
    packet.packetSize = sizeof(Header);
    packet.sequenceId = sequenceId;
    return packet;
}

::LoginResponsePacket NetworkMessage::createLoginResponsePacket(
    uint32_t playerId, uint16_t mapWidth, uint16_t mapHeight,
    uint32_t sequenceId)
{
    ::LoginResponsePacket packet = {};
    packet.header.opCode = OpCode::S2C_LOGIN_OK;
    packet.header.packetSize = sizeof(LoginResponsePacket);
    packet.header.sequenceId = sequenceId;
    packet.playerId = playerId;
    packet.mapWidth = mapWidth;
    packet.mapHeight = mapHeight;
    return packet;
}

::EntitySpawnPacket NetworkMessage::createEntitySpawnPacket(uint32_t entityId,
                                                            uint8_t type,
                                                            float x, float y,
                                                            uint32_t sequenceId)
{
    ::EntitySpawnPacket packet = {};
    packet.header.opCode = OpCode::S2C_ENTITY_NEW;
    packet.header.packetSize = sizeof(EntitySpawnPacket);
    packet.header.sequenceId = sequenceId;
    packet.entityId = entityId;
    packet.type = type;
    packet.x = x;
    packet.y = y;
    return packet;
}

::EntityPositionPacket NetworkMessage::createEntityPositionPacket(
    uint32_t entityId, float x, float y, uint32_t sequenceId)
{
    ::EntityPositionPacket packet = {};
    packet.header.opCode = OpCode::S2C_ENTITY_POS;
    packet.header.packetSize = sizeof(EntityPositionPacket);
    packet.header.sequenceId = sequenceId;
    packet.entityId = entityId;
    packet.x = x;
    packet.y = y;
    return packet;
}

::ScoreUpdatePacket NetworkMessage::createScoreUpdatePacket(uint32_t score,
                                                            uint32_t sequenceId)
{
    ::ScoreUpdatePacket packet = {};
    packet.header.opCode = OpCode::S2C_SCORE_UPDATE;
    packet.header.packetSize = sizeof(ScoreUpdatePacket);
    packet.header.sequenceId = sequenceId;
    packet.score = score;
    return packet;
}

::EntityDeadPacket NetworkMessage::createEntityDeadPacket(uint32_t entityId,
                                                          uint32_t sequenceId)
{
    ::EntityDeadPacket packet = {};
    packet.header.opCode = OpCode::S2C_ENTITY_DEAD;
    packet.header.packetSize = sizeof(EntityDeadPacket);
    packet.header.sequenceId = sequenceId;
    packet.entityId = entityId;
    return packet;
}

bool NetworkMessage::validatePacket(const void* data, size_t size,
                                    uint8_t expectedOpCode)
{
    if (!data || size < sizeof(Header)) {
        return false;
    }

    const Header* header = static_cast<const Header*>(data);

    if (header->packetSize > size || header->packetSize < sizeof(Header)) {
        return false;
    }

    if (expectedOpCode != 0 && header->opCode != expectedOpCode) {
        return false;
    }

    if (header->opCode == 0 ||
        (header->opCode > C2S_INPUT && header->opCode < S2C_LOGIN_OK) ||
        header->opCode > S2C_SCORE_UPDATE) {
        return false;
    }

    return true;
}

uint16_t NetworkMessage::getPacketSize(const void* data, size_t dataSize)
{
    if (!data || dataSize < sizeof(Header)) {
        return 0;
    }
    const Header* header = static_cast<const Header*>(data);
    return header->packetSize;
}

uint32_t NetworkMessage::getSequenceId(const void* data, size_t dataSize)
{
    if (!data || dataSize < sizeof(Header)) {
        return 0;
    }
    const Header* header = static_cast<const Header*>(data);
    return header->sequenceId;
}

uint8_t NetworkMessage::getOpCode(const void* data, size_t dataSize)
{
    if (!data || dataSize < sizeof(Header)) {
        return 0;
    }
    const Header* header = static_cast<const Header*>(data);
    return header->opCode;
}

std::string NetworkMessage::inputMaskToString(uint8_t inputMask)
{
    if (inputMask == 0) {
        return "NONE";
    }

    std::stringstream ss;
    bool first = true;

    if (inputMask & InputMask::UP) {
        if (!first) ss << "+";
        ss << "UP";
        first = false;
    }
    if (inputMask & InputMask::DOWN) {
        if (!first) ss << "+";
        ss << "DOWN";
        first = false;
    }
    if (inputMask & InputMask::LEFT) {
        if (!first) ss << "+";
        ss << "LEFT";
        first = false;
    }
    if (inputMask & InputMask::RIGHT) {
        if (!first) ss << "+";
        ss << "RIGHT";
        first = false;
    }
    if (inputMask & InputMask::SHOOT) {
        if (!first) ss << "+";
        ss << "SHOOT";
        first = false;
    }

    return ss.str();
}

std::string NetworkMessage::entityTypeToString(uint8_t entityType)
{
    switch (entityType) {
        case EntityType::PLAYER:
            return "PLAYER";
        case EntityType::BYDOS:
            return "BYDOS";
        case EntityType::MISSILE:
            return "MISSILE";
        default:
            return "UNKNOWN";
    }
}

std::string NetworkMessage::opCodeToString(uint8_t opCode)
{
    switch (opCode) {
        case C2S_LOGIN:
            return "C2S_LOGIN";
        case C2S_ACK:
            return "C2S_ACK";
        case C2S_DISCONNECT:
            return "C2S_DISCONNECT";
        case C2S_INPUT:
            return "C2S_INPUT";
        case S2C_LOGIN_OK:
            return "S2C_LOGIN_OK";
        case S2C_ENTITY_NEW:
            return "S2C_ENTITY_NEW";
        case S2C_ENTITY_POS:
            return "S2C_ENTITY_POS";
        case S2C_ENTITY_DEAD:
            return "S2C_ENTITY_DEAD";
        case S2C_SCORE_UPDATE:
            return "S2C_SCORE_UPDATE";
        default:
            return "UNKNOWN";
    }
}

}  // namespace rtype
