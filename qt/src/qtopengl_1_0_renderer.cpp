#include <qt/qtopengl_1_0_renderer.h>

namespace simplex
{
namespace qt
{

QtOpenGL_1_0_Renderer::QtOpenGL_1_0_Renderer(const QOpenGLContext *context)
{
    setOwningContext(context);
    if (!initializeOpenGLFunctions())
    {
        // log...
        assert(false);
    }
}

void QtOpenGL_1_0_Renderer::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}

void QtOpenGL_1_0_Renderer::render()
{
    glClearColor(.5f, .5f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex2f(-.5f, -.7f);
    glColor3f(0.f, 1.f, 0.f);
    glVertex2f(.5f, -.7f);
    glColor3f(0.f, 0.f, 1.f);
    glVertex2f(0.f, .5f);
    glEnd();
}

}
}
