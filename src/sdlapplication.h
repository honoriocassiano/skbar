#include "application.h"

#include <SDL2/SDL.h>

namespace skbar {

class SDLApplication : public Application {

public:
    SDLApplication(const std::string &title, int width, int height);

    virtual ~SDLApplication();

    void Initialize() override;

    void LoadMesh(const std::string &filename) override;

    void SaveMesh(const std::string &filename) override;


private:
    void InitializeSDL();

    void InitializeGL();

private:
    SDL_Window *window;
    SDL_GLContext glContext;

    bool running;

    const std::string title;
    const int width;
    const int height;
};
}
