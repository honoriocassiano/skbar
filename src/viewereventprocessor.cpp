#include "viewereventprocessor.h"

#include "sdlevent.h"

skbar::ViewerEventProcessor::ViewerEventProcessor(Viewer *_viewer)
    : viewer(_viewer), rotating(false), dragging(false) {

}

skbar::ViewerEventProcessor::~ViewerEventProcessor() {

}

bool skbar::ViewerEventProcessor::Process(const skbar::Event &_event) {
    auto event = dynamic_cast<const SDLEvent &>(_event).Get();

    bool processed = false;

    if (event->type == SDL_MOUSEBUTTONDOWN) {
        processed = ProcessMouseButtonDown(event);
    } else if (event->type == SDL_MOUSEBUTTONUP) {
        processed = ProcessMouseButtonUp(event);
    } else if (event->type == SDL_MOUSEMOTION) {
        processed = ProcessMouseMotion(event);
    } else if (event->type == SDL_MOUSEWHEEL) {
        processed = ProcessMouseWheel(event);
    }

    return processed;
}

bool skbar::ViewerEventProcessor::ProcessMouseButtonDown(const SDL_Event *event) {

    switch (event->button.button) {

        case SDL_BUTTON_LEFT:
            rotating = true;
            dragging = false;

            break;

        case SDL_BUTTON_RIGHT:
            rotating = false;
            dragging = true;

            break;

        default:
            return false;
    }

    return true;
}

bool skbar::ViewerEventProcessor::ProcessMouseButtonUp(const SDL_Event *event) {

    switch (event->button.button) {

        case SDL_BUTTON_LEFT:
            rotating = false;
            break;

        case SDL_BUTTON_RIGHT:
            dragging = false;
            break;

        default:
            return false;
    }

    return true;
}

bool skbar::ViewerEventProcessor::ProcessMouseWheel(const SDL_Event *event) {

    viewer->GetCamera().Zoom(event->wheel.y);

    return true;
}

bool skbar::ViewerEventProcessor::ProcessMouseMotion(const SDL_Event *event) {

    bool processed = false;

    auto motion = event->motion;

    if (rotating) {

        auto dx = float(motion.xrel);
        auto dy = float(-motion.yrel);

        float dtheta = (dx / viewer->GetWidth()) * 2 * M_PI;
		float dphi = (dy / viewer->GetHeight()) * M_PI;

        viewer->GetCamera().Rotate(dtheta, dphi);

        processed = true;

    } else if (dragging) {

        auto dx = float(motion.xrel) / 100;
		auto dy = float(-motion.yrel) / 100;

        viewer->GetCamera().Drag(dx, dy);

        processed = true;
    } else {
        // TODO Process drawing here
    }

    return processed;
}

bool skbar::ViewerEventProcessor::ProcessKeyboard(const SDL_Event *event) {

}
