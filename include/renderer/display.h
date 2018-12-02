#ifndef DISPLAY_H
#define DISPLAY_H

#include "../utils/singletoon.h"
#include "../utils/pimpl.h"
#include "renderer_global.h"
#include "forwarddecl.h"

namespace renderer {

class DisplayPrivate;
class RENDERERSHARED_EXPORT Display {
    SINGLETON(Display)
    PIMPL(Display)

public:
    int32_t eglMinorVersion() const;
    int32_t eglMajorVersion() const;

private:
    Display();
    ~Display();

    DisplayPrivate *m_;
};

class DisplayPixelFormatPrivate;
class RENDERERSHARED_EXPORT DisplayPixelFormat {
    PIMPL(DisplayPixelFormat)

public:
    ~DisplayPixelFormat();

    int32_t redSize() const;
    int32_t greenSize() const;
    int32_t blueSize() const;
    int32_t alphaSize() const;
    int32_t depthSize() const;
    int32_t stencilSize() const;

    static DisplayPixelFormatsList pixelFormatsList();
    static DisplayPixelFormatPtr choosePixelFormat(int32_t r = 8, int32_t g = 8, int32_t b = 8, int32_t a = 8, int32_t d = 24, int32_t s = 8);

private:
    DisplayPixelFormat();

    DisplayPixelFormatPrivate *m_;
};

class WindowSurfacePrivate;
class RENDERERSHARED_EXPORT WindowSurface
{
    PIMPL(WindowSurface)

public:
    ~WindowSurface();

    DisplayPixelFormatPtr pixelFormat() const;
    void swapBuffers();

    static WindowSurfacePtr createWindowSurface(DisplayPixelFormatPtr pixelFormat, intptr_t windowID);

private:
    WindowSurface(DisplayPixelFormatPtr pixelFormat);

    WindowSurfacePrivate *m_;
};

}

#endif // DISPLAY_H
