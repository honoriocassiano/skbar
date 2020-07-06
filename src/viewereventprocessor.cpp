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
    } else if (event->type == SDL_MOUSEMOTION) {
        processed = ProcessMouseMotion(event);
    } else if (event->type == SDL_MOUSEWHEEL) {
        processed = ProcessMouseWheel(event);
    }
}

bool skbar::ViewerEventProcessor::ProcessMouseButtonDown(const SDL_Event *event) {

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

}

bool skbar::ViewerEventProcessor::ProcessMouseMotion(const SDL_Event *event) {

}

bool skbar::ViewerEventProcessor::ProcessKeyboard(const SDL_Event *event) {

}
