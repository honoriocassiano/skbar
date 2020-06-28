// Local headers
#include "framebuffer.h"

// Standard library headers
#include <string>

namespace skbar {

class Viewer {

public:
    Viewer(int width, int height);

    virtual ~Viewer();

    /** Initialize OpenGL parameters
     */
    void Initialize();

    /** Render the viewer if NeedRender() returns true; otherwhise
     * render the content in framebuffer
     */
    void Render();

    /* Resize the viewer and any other renderable
     *
     * @param width new width
     * @param height new height
     */
    void Resize(int width, int height);

    /** Update the state of viewer.
     *
     * According to the new state, needRender can be set to true
     */
    void Update(/* TODO Add parameters */);

    /** Apply zoom to the scene
     *
     * @param zoom level of zoom
     */
    void Zoom(float zoom);

    /** Load a mesh into viewer
     */
    void LoadMesh(const std::string &filename);

    /* TODO Add methods to change visualization */

private:

    // Return if need to re-render the viewer or any other renderable
    bool NeedRender() const;

private:

    // Window size
    int width;
    int height;

    // GL variables
    float bgColor[4];

    // Mouse motion state
    bool rotating;
    bool dragging;

    // True if something changes
    bool needRender;

    // Framebuffer to optimize rendering process
    FrameBuffer *frameBuffer;
};
}
