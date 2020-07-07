#ifndef SDLAPPLICATION_H_
#define SDLAPPLICATION_H_

#include "application.h"
#include "eventprocessor.h"
#include "viewer.h"

#include <SDL2/SDL.h>

namespace skbar {

class SDLApplication : public Application {

public:

    /**
     * Constructor
     *
     * @param title title of window
     * @param width width of window
     * @param height height of window
     */
    SDLApplication(const std::string &title, int width, int height);

    virtual ~SDLApplication();

    /**
     * Initialize all components (SDL, OpenGL, Camera...)
     */
    void Initialize() override;

    /**
     * Load a mesh by filename
     *
     * @param filename name of model file
     */
    void LoadMesh(const std::string &filename) override;

    /**
     * Save current mesh to .obj
     *
     * @param filename name to save te file
     */
    void SaveMesh(const std::string &filename) override;

    /**
     * Run the application
     */
    void Run();

private:

    /**
     * Initialize SDL
     */
    void InitializeSDL();

    /**
     * Initialize OpenGL
     */
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

    // SDL window itself
    SDL_Window *window;

    // OpenGL context of SDL window
    SDL_GLContext glContext;

    // Process events not relative to the window
    EventProcessor eventProcessor;

    // If application still running
    bool running;

    // Window title
    const std::string title;

    // Window dimensions
    int width;
    int height;

    // Contents of the window
    Viewer *viewer;
};
}

#endif
