#include "drawereventprocessor.h"

#include "glcamera.h"
#include "gldrawer.h"
#include "opsketch.h"

skbar::DrawerEventProcessor::DrawerEventProcessor(Drawer *_drawer)
    : drawer(_drawer), drawing(false) {

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
    // auto glDrawer = dynamic_cast<GLDrawer *>(drawer);

    bool processed = false;

    if (button.button == SDL_BUTTON_LEFT) {

        processed = AddPointToSketch(button.x, button.y);

        // If point was added, so user is drawing and not rotating or dragging
        drawing = processed;
    }

    return processed;
}

bool skbar::DrawerEventProcessor::ProcessMouseButtonUp(const SDL_Event *event) {

    auto processed = false;

    if (event->button.button == SDL_BUTTON_LEFT) {

        // If user release the left button, it can continue drawing
        // if press mouse button over the mesh again
        drawing = false;

    } else if (event->button.button == SDL_BUTTON_RIGHT) {

        drawing = false;

        // If a sketch is started and not closed, close it and disable drawing
        if (drawer->IsStarted()) {

            auto glDrawer = dynamic_cast<GLDrawer *>(drawer);

            assert((glDrawer != nullptr) && "Drawer is not an instance of GLDrawer");

            auto sketch = dynamic_cast<OPSketch &>(glDrawer->GetMesh().GetSketch());

            sketch.Close();

            processed = true;
        }
    }

    return processed;
}

bool skbar::DrawerEventProcessor::CheckMouseState(unsigned int button) {

    auto mouseState = SDL_GetMouseState(nullptr, nullptr);

    return static_cast<bool>(mouseState & SDL_BUTTON(button));
}

bool skbar::DrawerEventProcessor::ProcessMouseMotion(const SDL_Event *event) {

    auto motion = event->motion;
    auto processed = false;

    if (CheckMouseState(SDL_BUTTON_LEFT) && drawing) {
        processed = AddPointToSketch(motion.x, motion.y);
    }

    return processed;
}

bool skbar::DrawerEventProcessor::AddPointToSketch(int x, int y) {

    auto processed = false;

    auto glDrawer = dynamic_cast<GLDrawer *>(drawer);
    auto camera = dynamic_cast<const GLCamera &>(glDrawer->GetCamera());

    auto ray = camera.PositionToRay(x, y);

    auto intersection = drawer->GetClosestIntersection(ray);

    if (intersection.Intersect()) {

        auto sketch = dynamic_cast<OPSketch &>(glDrawer->GetMesh().GetSketch());

        if (!sketch.IsStarted()) {
            sketch.Start();
        }

        processed = sketch.AddPoint(ray, intersection);
    }

    return processed;
}