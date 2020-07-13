#include "viewer.h"

#include "viewereventprocessor.h"
#include "gldrawer.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

skbar::Viewer::Viewer(int _width, int _height)
    : width(_width), height(_height), bgColor{.3f, .3f, .3f, 1},
      rotating(false), dragging(false), needRender(false),
      frameBuffer(nullptr), camera(nullptr), eventProcessor(nullptr) {
}

skbar::Viewer::~Viewer() {
    delete frameBuffer;
    delete camera;
    delete eventProcessor;
    delete drawer;

    frameBuffer = nullptr;
    camera = nullptr;
    eventProcessor = nullptr;
    drawer = nullptr;
}

void skbar::Viewer::Initialize() {

    frameBuffer = new FrameBuffer(width, height);
    camera = new GLCamera(width, height);
    eventProcessor = new ViewerEventProcessor(this);
    drawer = new GLDrawer(this);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	const float ambient[] { 0.2, 0.2, 0.2, 1.0 };
	const float diffuse[] { 0.8, 0.8, 0.8, 1.0 };
	const float specular[] { 0.5, 0.5, 0.5, 1.0 };
	const float position[] { 1.0, 1.0, -4.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glDisable(GL_LIGHTING);

    needRender = true;
}

void skbar::Viewer::Render() {

    camera->Render();

    if (NeedRender()) {

        // Bind screen on write mode
        frameBuffer->Bind(FrameBuffer::Mode::WRITE);

        /// Clear color buffers
        glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mesh.Render();

        // Unbind framework on read mode
        frameBuffer->Unbind(FrameBuffer::Mode::WRITE);
    }

    // Bind framebuffer on read mode
    frameBuffer->Bind(FrameBuffer::Mode::READ);

    // Bind framebuffer of size (width, height) on rect (0, 0, width, height)
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
	                  GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    // Unbind framebuffer on read mode
    frameBuffer->Unbind(FrameBuffer::Mode::READ);

    needRender = false;
}

void skbar::Viewer::Resize(int _width, int _height) {

    if ((width != _width) || (height != _height)) {
        width = _width;
        height = _height;

        frameBuffer->Unbind(FrameBuffer::Mode::READ_WRITE);

        delete frameBuffer;

        frameBuffer = new FrameBuffer(width, height);

        camera->Resize(width, height);

        needRender = true;
    }
}

void skbar::Viewer::Update(/* TODO Add parameters */) {
#warning "Implement this"
}

bool skbar::Viewer::NeedRender() const {
#warning "Improve this"
    return true;
}

void skbar::Viewer::LoadMesh(const std::string &filename) {
    mesh.Load(filename);

    ApplyBBox(mesh.GetBBox());
}

void skbar::Viewer::ApplyBBox(const BBox &bbox) {
    camera->SetBox(bbox);
}

bool skbar::Viewer::ProcessEvent(const skbar::Event &event) {
    return eventProcessor->Process(event);
}
