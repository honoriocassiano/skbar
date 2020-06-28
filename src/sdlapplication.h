#include "application.h"
#include "eventprocessor.h"

#include <SDL2/SDL.h>

namespace skbar {

class SDLApplication : public Application {

public:
    SDLApplication(const std::string &title, int width, int height);

    virtual ~SDLApplication();

    void Initialize() override;

    void LoadMesh(const std::string &filename) override;

    void SaveMesh(const std::string &filename) override;

    void Run();

private:
    void InitializeSDL();

    void InitializeGL();

    /** Update states before render
     */
    void Update();

    /** Render new state
     */
    void Render();

    /** Process mouse and keyboard events
     */
    void ProcessEvents();

private:
    SDL_Window *window;
    SDL_GLContext glContext;

    EventProcessor eventProcessor;

    bool running;

    const std::string title;
    int width;
    int height;
};
}
