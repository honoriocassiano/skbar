#ifndef SDLEVENT_H
#define SDLEVENT_H

#include "baseeventprocessor.h"

#include <SDL2/SDL.h>

namespace skbar {

class SDLEvent : public Event {

public:
    SDLEvent(const SDL_Event *_event) : event(_event) {}
    virtual ~SDLEvent() {}

    const SDL_Event* Get() const { return event; }

private:
    const SDL_Event* event;
};
}

#endif /* SDLEVENT_H */
