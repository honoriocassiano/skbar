#include "drawereventprocessor.h"

#include "glcamera.h"
#include "gldrawer.h"
#include "opsketch.h"

skbar::DrawerEventProcessor::DrawerEventProcessor(Drawer *drawer) : drawing(false) {

}

skbar::DrawerEventProcessor::~DrawerEventProcessor() {

}

bool skbar::DrawerEventProcessor::Process(const Event &_event) {

    auto event = dynamic_cast<const SDLEvent &>(_event).Get();

    bool processed = false;

    if (event->type == SDL_MOUSEBUTTONDOWN) {
        processed = ProcessMouseButtonDown(event);
    } else if (event->type == SDL_MOUSEBUTTONUP) {
        processed = ProcessMouseButtonUp(event);
    } else if (event->type == SDL_MOUSEMOTION) {
        processed = ProcessMouseMotion(event);
    }

    return processed;
}

bool skbar::DrawerEventProcessor::ProcessMouseButtonDown(const SDL_Event *event) {

    auto button = event->button;
    auto glDrawer = dynamic_cast<GLDrawer *>(drawer);

    bool processed = false;

    if (button.button == SDL_BUTTON_LEFT) {

        auto camera = dynamic_cast<const GLCamera &>(glDrawer->GetCamera());

        auto ray = camera.PositionToRay(button.x, button.y);

        if (false) {
            auto sketch = dynamic_cast<OPSketch &>(glDrawer->GetMesh().GetSketch());

            if (sketch.AddPoint(ray)) {
                drawing = true;
                processed = true;
            }
        }
    }

    return processed;
}

bool skbar::DrawerEventProcessor::ProcessMouseButtonUp(const SDL_Event *event) {

    drawing = true;

    if (event->button.button == SDL_BUTTON_RIGHT) {
        drawing = false;
    }

    // TODO Close the drawing

    return false;
}

bool skbar::DrawerEventProcessor::CheckMouseState() {

}

bool skbar::DrawerEventProcessor::ProcessMouseMotion(const SDL_Event *event) {

}
