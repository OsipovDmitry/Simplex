#ifndef QTOPENGL_1_0_RENDERER_H
#define QTOPENGL_1_0_RENDERER_H

#include <QOpenGLFunctions_1_0>

#include <utils/noncopyble.h>

#include <core/igraphicsrenderer.h>

#include <qt/qtglobal.h>

namespace simplex
{
namespace qt
{

class QT_SHARED_EXPORT QtOpenGL_1_0_Renderer : public core::IGraphicsRenderer, protected QOpenGLFunctions_1_0
{
    NONCOPYBLE(QtOpenGL_1_0_Renderer)

public:
    QtOpenGL_1_0_Renderer(const QOpenGLContext *);

    void resize(int, int);
    void render();

};

}
}

#endif // QTOPENGL_1_0_RENDERER_H
