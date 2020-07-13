#ifndef DRAWEREVENTPROCESSOR_H
#define DRAWEREVENTPROCESSOR_H

#include "baseeventprocessor.h"
#include "drawer.h"
#include "sdlevent.h"

namespace skbar {

class DrawerEventProcessor : public BaseEventProcessor {

public:
    DrawerEventProcessor(Drawer *drawer);

    virtual ~DrawerEventProcessor();

    bool Process(const Event &event) override;

    bool ProcessMouseButtonDown(const SDL_Event *event);

    bool ProcessMouseButtonUp(const SDL_Event *event);

    bool ProcessMouseMotion(const SDL_Event *event);

    bool CheckMouseState();

private:
    Drawer *drawer;

    bool drawing;

};
}

#endif /* DRAWEREVENTPROCESSOR_H */
