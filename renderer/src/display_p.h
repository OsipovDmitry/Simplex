#ifndef DISPLAY_P_H
#define DISPLAY_P_H

#include <EGL/egl.h>

#include <renderer/forwarddecl.h>

namespace renderer {

class DisplayPrivate {
public:
    EGLDisplay display;
    EGLint majorVersion, minorVersion;
    DisplayPrivate() :
        display(nullptr),
        majorVersion(0),
        minorVersion(0)
    {}
};

class DisplayPixelFormatPrivate {
public:
    EGLConfig config;

    DisplayPixelFormatPrivate() :
        config(nullptr)
    {}
};

class WindowSurfacePrivate {
public:
    DisplayPixelFormatPtr pixelFormat;
    EGLSurface surface;

    WindowSurfacePrivate(DisplayPixelFormatPtr pf) :
        pixelFormat(pf),
        surface(nullptr)
    {}
};

}

#endif // DISPLAY_P_H
