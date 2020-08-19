#ifndef VIEWEREVENTPROCESSOR_H
#define VIEWEREVENTPROCESSOR_H

#include "baseeventprocessor.h"
#include "viewer.h"

#include <SDL2/SDL.h>

namespace skbar {

class ViewerEventProcessor : public BaseEventProcessor {

public:
    ViewerEventProcessor(Viewer *viewer);
    virtual ~ViewerEventProcessor();

    bool Process(const Event &event) override;

private:
    bool ProcessMouseButtonDown(const SDL_Event *event);

    bool ProcessMouseButtonUp(const SDL_Event *event);

    bool ProcessMouseWheel(const SDL_Event *event);

    bool ProcessMouseMotion(const SDL_Event *event);

    bool ProcessKeyboardUp(const SDL_Event *event);

private:
    Viewer *viewer;

    bool rotating;
    bool dragging;
};
}

#endif /* VIEWEREVENTPROCESSOR_H */
