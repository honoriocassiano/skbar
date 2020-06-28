#include "sdlapplication.h"

// Installed libraries
#include <GL/glew.h>

// Standard Libraries
#include <stdexcept>

// Event filter
int RepeatedKeyEventFilter(void *userdata, SDL_Event *event) {
	if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {
		// Return true if not repeated
		return !event->key.repeat;
	}

	return true;
}

skbar::SDLApplication::SDLApplication(const std::string &_title, int _width, int _height) :
    title(_title), width(_width), height(_height) {

}

skbar::SDLApplication::~SDLApplication() {

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void skbar::SDLApplication::Initialize() {
    InitializeSDL();
    InitializeGL();
}

void skbar::SDLApplication::LoadMesh(const std::string &filename) {

}

void skbar::SDLApplication::SaveMesh(const std::string &filename) {

}

void skbar::SDLApplication::InitializeSDL() {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		throw std::runtime_error(std::string("Error: %s\n") + SDL_GetError());
	}

#if __APPLE__
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);// Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_WindowFlags window_flags = (SDL_WindowFlags) (SDL_WINDOW_OPENGL
			| SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	window = SDL_CreateWindow(title.c_str(),
	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
			window_flags);
	glContext = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	SDL_SetEventFilter(RepeatedKeyEventFilter, nullptr);

	SDL_GL_MakeCurrent(window, glContext);

}

void skbar::SDLApplication::InitializeGL() {

    if (glewInit() != GLEW_OK) {
		throw std::runtime_error("Failed to initialize GLEW!");
	}
}

void skbar::SDLApplication::Run() {
    running = true;

	SDL_MaximizeWindow(window);

    while (running) {
        Update();

        Render();
    }
}

void skbar::SDLApplication::Update() {
    ProcessEvents();
}

void skbar::SDLApplication::Render() {
    SDL_GL_SwapWindow(window);
}


void skbar::SDLApplication::ProcessEvents() {

    SDL_Event event;
    const Uint8 *keyboardState = SDL_GetKeyboardState(nullptr);

    while (SDL_PollEvent(&event)) {

        eventProcessor.ProcessRealTimeKeyboard(keyboardState);

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {

                case SDLK_ESCAPE:
                    running = false;
                    break;

                default:
                    // TODO Add other events here
                    eventProcessor.ProcessKeyboard(event.key);
                    break;
            }
        }

        if (event.type == SDL_QUIT) {
            running = false;
        }

        if (event.type == SDL_WINDOWEVENT
                && (event.window.windowID == SDL_GetWindowID(window))) {

            switch (event.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    running = false;

                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        SDL_GetWindowSize(window, &width, &height);

                        // TODO Resize OpenGL

            }
        }

		// Get all mouse events
		switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEMOTION:
			case SDL_MOUSEWHEEL:
			case SDL_MOUSEWHEEL_FLIPPED:
			case SDL_MOUSEWHEEL_NORMAL:
			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
			case SDL_FINGERMOTION:
                eventProcessor.ProcessMouse(event);
				break;
			}
    }
}

