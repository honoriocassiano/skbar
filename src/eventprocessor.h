#include <SDL2/SDL.h>

namespace skbar {

class EventProcessor {

public:

    EventProcessor() {}

    virtual ~EventProcessor() {}

    /*
     * Process keyboard events
     */
    void ProcessKeyboard(const SDL_KeyboardEvent &event);

    /**
     * Process all realtime(repeated) keyboard events
     */
    void ProcessRealTimeKeyboard(const Uint8 *keyboardState);

    /**
     * Process all mouse related events (motion, key, wheel, ...)
     */
    void ProcessMouse(const SDL_Event &event);
};

}
