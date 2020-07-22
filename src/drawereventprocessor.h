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

private:

    bool Process(const Event &event) override;

    bool ProcessMouseButtonDown(const SDL_Event *event);

    bool ProcessMouseButtonUp(const SDL_Event *event);

    bool ProcessMouseMotion(const SDL_Event *event);

    bool ProcessKeyboard(const SDL_Event *event);

    bool CheckMouseState(unsigned int button);

    bool AddPointToSketch(int x, int y);

private:
    Drawer *drawer;

    // Indicate when user is drawing while keep mouse pressed
    bool drawing;

    Intersection<int, Vec3f> lastIntersection;

    Vec2i lastPoint;

    int epsilon;

};
}

#endif /* DRAWEREVENTPROCESSOR_H */
