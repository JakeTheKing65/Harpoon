#ifndef IEVENT_H
#define IEVENT_H

#include "utils/uuid.h"


class IEvent {
public:
	virtual UUID getEventUuid() const = 0;
};

#endif