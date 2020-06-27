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

	SDL_MaximizeWindow(window);
}

void skbar::SDLApplication::InitializeGL() {

    if (glewInit() != GLEW_OK) {
		throw std::runtime_error("Failed to initialize GLEW!");
	}
}
