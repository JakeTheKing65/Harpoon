#ifndef EVENTIRCJOINED_H
#define EVENTIRCJOINED_H

#include "../IEvent.hpp"
#include <string>


class EventIrcJoined : public IEvent {
	size_t userId;
	size_t serverId;
	std::string username;
	std::string channel;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcJoined(size_t userId, size_t serverId, const std::string& username, const std::string& channel);
	size_t getUserId() const;
	size_t getServerId() const;
	std::string getUsername() const;
	std::string getChannel() const;
};

#endif