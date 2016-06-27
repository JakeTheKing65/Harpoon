#ifndef EVENTIRCACTION_H
#define EVENTIRCACTION_H

#include "../IClientEvent.hpp"
#include <string>


class EventIrcAction : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string username;
    std::string channel;
    std::string message;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcAction(size_t userId,
                   size_t serverId,
                   std::string username,
                   std::string channel,
                   std::string message);

    size_t getUserId() const override;
    size_t getServerId() const;
    std::string getUsername() const;
    std::string getChannel() const;
    std::string getMessage() const;
};

#endif
