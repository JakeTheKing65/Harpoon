#include "EventIrcConnected.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcConnected::uuid = ::uuid.get();
UUID EventIrcConnected::getEventUuid() const {
    return this->uuid;
}

EventIrcConnected::EventIrcConnected(size_t userId, size_t serverId)
    : userId{userId}
    , serverId{serverId}
{
}

size_t EventIrcConnected::getUserId() const {
    return userId;
}

size_t EventIrcConnected::getServerId() const {
    return serverId;
}

