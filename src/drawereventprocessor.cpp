#include "drawereventprocessor.h"

#include "glcamera.h"
#include "gldrawer.h"
#include "opsketch.h"

skbar::DrawerEventProcessor::DrawerEventProcessor(Drawer *_drawer)
    : drawer(_drawer), drawing(false), lastIntersection(-1, Vec3f(), false), epsilon(5) {

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
    } else if (event->type == SDL_KEYUP) {
        processed = ProcessKeyboard(event);
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

        // TODO Uncomment this code
//        drawing = false;
//
//        // If a sketch is started and not closed, close it and disable drawing
//        if (drawer->IsStarted()) {
//
//            auto glDrawer = dynamic_cast<GLDrawer *>(drawer);
//
//            assert((glDrawer != nullptr) && "Drawer is not an instance of GLDrawer");
//
//            auto sketch = dynamic_cast<OPSketch &>(glDrawer->GetMesh().GetSketch());
//
//            sketch.Close();
//
//            processed = true;
//        }
    }

    return processed;
}

bool skbar::DrawerEventProcessor::ProcessKeyboard(const SDL_Event *event) {

    auto processed = false;

    switch (event->key.keysym.sym) {

        case SDLK_r:
            drawer->Reset();
            processed = true;

            break;

        default:
            break;
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

    auto deltha = Norm(Vec2i{lastPoint[0] - motion.x, lastPoint[1] - motion.y});

    if (CheckMouseState(SDL_BUTTON_LEFT) && drawing && deltha > epsilon) {

        if (lastIntersection.Intersect()) {
            processed = AddPointToSketch(motion.x, motion.y);
        }
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

        if ((lastIntersection.Pointer() != intersection.Pointer())) {
            auto& sketch = dynamic_cast<OPSketch &>(glDrawer->GetMesh().GetSketch());

            if (!sketch.IsStarted()) {
                sketch.Start();
            }

            const bool added = sketch.AddPoint(ray, intersection, camera.DirectionRay(), camera.GetProjection());

            if (added) {
                lastIntersection = intersection;
                lastPoint = Vec2i{x, y};
            }

            processed = sketch.IsStarted();
        } else {
            // Prevent rotation event if intersection is on same face that last intersection
            processed = true;
        }
    }

    return processed;
}