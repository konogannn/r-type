/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** NetworkMessage - Utilities for network message handling
*/

#include "NetworkMessage.hpp"

#include <cstring>
#include <sstream>

namespace rtype {

::LoginPacket NetworkMessage::createLoginPacket(const std::string& username,
                                                uint32_t sequenceId) {
    ::LoginPacket packet;
    packet.header.opCode = static_cast<uint8_t>(::OpCode::C2S_LOGIN);
    packet.header.packetSize = sizeof(::LoginPacket);
    packet.header.sequenceId = sequenceId;

    std::strncpy(packet.username, username.c_str(),
                 sizeof(packet.username) - 1);
    packet.username[sizeof(packet.username) - 1] = '\0';

    return packet;
}

::InputPacket NetworkMessage::createInputPacket(uint8_t inputMask,
                                                uint32_t sequenceId) {
    ::InputPacket packet;
    packet.header.opCode = static_cast<uint8_t>(::OpCode::C2S_INPUT);
    packet.header.packetSize = sizeof(::InputPacket);
    packet.header.sequenceId = sequenceId;
    packet.inputMask = inputMask;

    return packet;
}

::Header NetworkMessage::createDisconnectPacket(uint32_t sequenceId) {
    ::Header packet;
    packet.opCode = static_cast<uint8_t>(::OpCode::C2S_DISCONNECT);
    packet.packetSize = sizeof(::Header);
    packet.sequenceId = sequenceId;

    return packet;
}

::Header NetworkMessage::createAckPacket(uint32_t sequenceId) {
    ::Header packet;
    packet.opCode = static_cast<uint8_t>(::OpCode::C2S_ACK);
    packet.packetSize = sizeof(::Header);
    packet.sequenceId = sequenceId;

    return packet;
}

bool NetworkMessage::validatePacket(const void* data, size_t size,
                                    uint8_t expectedOpCode) {
    if (!data || size < sizeof(::Header)) {
        return false;
    }

    const ::Header* header = static_cast<const ::Header*>(data);

    if (header->packetSize > size) {
        return false;
    }

    if (expectedOpCode != 0 && header->opCode != expectedOpCode) {
        return false;
    }

    if (header->opCode == 0 ||
        header->opCode > static_cast<uint8_t>(::OpCode::S2C_SCORE_UPDATE)) {
        return false;
    }

    return true;
}

uint16_t NetworkMessage::getPacketSize(const void* data, size_t dataSize) {
    if (!data || dataSize < sizeof(::Header)) {
        return 0;
    }

    const ::Header* header = static_cast<const ::Header*>(data);
    return header->packetSize;
}

uint32_t NetworkMessage::getSequenceId(const void* data, size_t dataSize) {
    if (!data || dataSize < sizeof(::Header)) {
        return 0;
    }

    const ::Header* header = static_cast<const ::Header*>(data);
    return header->sequenceId;
}

uint8_t NetworkMessage::getOpCode(const void* data, size_t dataSize) {
    if (!data || dataSize < sizeof(::Header)) {
        return 0;
    }

    const ::Header* header = static_cast<const ::Header*>(data);
    return header->opCode;
}

std::string NetworkMessage::inputMaskToString(uint8_t inputMask) {
    std::vector<std::string> inputs;

    if (inputMask & InputMask::UP) {
        inputs.push_back("UP");
    }
    if (inputMask & InputMask::DOWN) {
        inputs.push_back("DOWN");
    }
    if (inputMask & InputMask::LEFT) {
        inputs.push_back("LEFT");
    }
    if (inputMask & InputMask::RIGHT) {
        inputs.push_back("RIGHT");
    }
    if (inputMask & InputMask::SHOOT) {
        inputs.push_back("SHOOT");
    }

    if (inputs.empty()) {
        return "NONE";
    }

    std::ostringstream oss;
    for (size_t i = 0; i < inputs.size(); ++i) {
        if (i > 0) {
            oss << "+";
        }
        oss << inputs[i];
    }

    return oss.str();
}

std::string NetworkMessage::entityTypeToString(uint8_t entityType) {
    switch (entityType) {
        case EntityType::PLAYER:
            return "PLAYER";
        case EntityType::BYDOS:
            return "BYDOS";
        case EntityType::MISSILE:
            return "MISSILE";
        default:
            return "UNKNOWN(" + std::to_string(entityType) + ")";
    }
}

std::string NetworkMessage::opCodeToString(uint8_t opCode) {
    switch (static_cast<::OpCode>(opCode)) {
        case ::OpCode::C2S_LOGIN:
            return "C2S_LOGIN";
        case ::OpCode::C2S_ACK:
            return "C2S_ACK";
        case ::OpCode::C2S_DISCONNECT:
            return "C2S_DISCONNECT";
        case ::OpCode::C2S_INPUT:
            return "C2S_INPUT";
        case ::OpCode::S2C_LOGIN_OK:
            return "S2C_LOGIN_OK";
        case ::OpCode::S2C_ENTITY_NEW:
            return "S2C_ENTITY_NEW";
        case ::OpCode::S2C_ENTITY_POS:
            return "S2C_ENTITY_POS";
        case ::OpCode::S2C_ENTITY_DEAD:
            return "S2C_ENTITY_DEAD";
        case ::OpCode::S2C_SCORE_UPDATE:
            return "S2C_SCORE_UPDATE";
        default:
            return "UNKNOWN(" + std::to_string(opCode) + ")";
    }
}

}  // namespace rtype
