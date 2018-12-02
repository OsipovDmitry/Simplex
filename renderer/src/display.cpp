#include <EGL/egl.h>

#include <renderer/display.h>
#include <logger/logger.h>

#include "display_p.h"

namespace renderer {

int32_t Display::eglMinorVersion() const
{
    return m_->minorVersion;
}

int32_t Display::eglMajorVersion() const
{
    return m_->majorVersion;
}

Display::Display() :
    m_(new DisplayPrivate)
{
    auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOG_CRITICAL("Failed to get display");
        return;
    }

    EGLint majorVer, minorVer;
    if (eglInitialize(display, &majorVer, &minorVer) == EGL_FALSE) {
        auto error = eglGetError();
        if (error == EGL_BAD_DISPLAY)
            LOG_CRITICAL("Failed to initialize EGL for display: Invalid display");
        else if (error == EGL_NOT_INITIALIZED)
            LOG_CRITICAL("Failed to initialize EGL for display: EGL can not be initialized");
        return;
    }

    m_->display = display;
    m_->majorVersion = majorVer;
    m_->minorVersion = minorVer;
}

Display::~Display()
{
    auto display = m_->display;
    delete m_;

    if (eglTerminate(display) == EGL_FALSE) {
        auto error = eglGetError();
        if (error == EGL_BAD_DISPLAY)
            LOG_CRITICAL("Failed to deinitialize EGL for display: Invalid display");
        return;
    }
}

DisplayPixelFormat::~DisplayPixelFormat()
{
    delete m_;
}

int32_t DisplayPixelFormat::redSize() const
{
    EGLint value = -1;
    eglGetConfigAttrib(Display::instance().m()->display, m_->config, EGL_RED_SIZE, &value);
    return value;
}

int32_t DisplayPixelFormat::greenSize() const
{
    EGLint value = -1;
    eglGetConfigAttrib(Display::instance().m()->display, m_->config, EGL_GREEN_SIZE, &value);
    return value;
}

int32_t DisplayPixelFormat::blueSize() const
{
    EGLint value = -1;
    eglGetConfigAttrib(Display::instance().m()->display, m_->config, EGL_BLUE_SIZE, &value);
    return value;
}

int32_t DisplayPixelFormat::alphaSize() const
{
    EGLint value = -1;
    eglGetConfigAttrib(Display::instance().m()->display, m_->config, EGL_ALPHA_SIZE, &value);
    return value;
}

int32_t DisplayPixelFormat::depthSize() const
{
    EGLint value = -1;
    eglGetConfigAttrib(Display::instance().m()->display, m_->config, EGL_DEPTH_SIZE, &value);
    return value;
}

int32_t DisplayPixelFormat::stencilSize() const
{
    EGLint value = -1;
    eglGetConfigAttrib(Display::instance().m()->display, m_->config, EGL_STENCIL_SIZE, &value);
    return value;
}

DisplayPixelFormatsList DisplayPixelFormat::pixelFormatsList()
{
    DisplayPixelFormatsList pixelFormatsList;

    EGLint numConfigs;
    if (eglGetConfigs(Display::instance().m()->display, nullptr, 0, &numConfigs) == EGL_FALSE) {
        auto error = eglGetError();
        // ...
        LOG_ERROR("Can not get list of pixel formats");
        return pixelFormatsList;
    }

    std::vector<EGLConfig> configs(numConfigs, nullptr);
    eglGetConfigs(Display::instance().m()->display, configs.data(), numConfigs, &numConfigs);
    for (auto config: configs) {
        if (!config)
            continue;
        DisplayPixelFormatPtr pixelFormatPtr(new DisplayPixelFormat());
        pixelFormatPtr->m_->config = config;
        pixelFormatsList.push_back(pixelFormatPtr);
    }

    return pixelFormatsList;
}

DisplayPixelFormatPtr DisplayPixelFormat::choosePixelFormat(int32_t r, int32_t g, int32_t b, int32_t a, int32_t d, int32_t s)
{
    EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_RED_SIZE, r,
        EGL_GREEN_SIZE, g,
        EGL_BLUE_SIZE, b,
        EGL_ALPHA_SIZE, a,
        EGL_DEPTH_SIZE, d,
        EGL_STENCIL_SIZE, s,
        EGL_NONE
    };
    EGLConfig config;
    EGLint numConfigs;
    if (eglChooseConfig(Display::instance().m()->display, attribs, &config, 1, &numConfigs) == EGL_FALSE) {
        auto error = eglGetError();
        // ...
        LOG_ERROR("Can not get pixel format");
        return nullptr;
    }

    if (numConfigs == 0) {
        LOG_ERROR("Can not get pixel format");
        return nullptr;
    }

    DisplayPixelFormatPtr pixelFormatPtr(new DisplayPixelFormat());
    pixelFormatPtr->m_->config = config;
    return pixelFormatPtr;
}

DisplayPixelFormat::DisplayPixelFormat() :
    m_(new DisplayPixelFormatPrivate())
{
}

WindowSurface::~WindowSurface()
{
    auto surface = m_->surface;
    auto pf = m_->pixelFormat;
    delete m_;

    if (eglDestroySurface(Display::instance().m()->display, surface) == EGL_FALSE) {
        auto error = eglGetError();
        // ...
        LOG_ERROR("Can not destory window surface");
        return;
    }
}

DisplayPixelFormatPtr WindowSurface::pixelFormat() const
{
    return m_->pixelFormat;
}

void WindowSurface::swapBuffers()
{
    if (eglSwapBuffers(Display::instance().m()->display,
                       m_->surface) == EGL_FALSE) {
        auto error = eglGetError();
        // ...
        LOG_ERROR("Can not swap color buffers");
        return;
    }
}

WindowSurfacePtr WindowSurface::createWindowSurface(DisplayPixelFormatPtr pixelFormat, intptr_t windowID)
{
    EGLint attribs[] = {
        EGL_NONE
    };

    auto surface = eglCreateWindowSurface(Display::instance().m()->display, pixelFormat->m()->config, (EGLNativeWindowType)windowID, attribs);
    if (surface == EGL_NO_SURFACE) {
        auto error = eglGetError();
        // ...
        LOG_ERROR("Can not create window surface");
        return nullptr;
    }

    WindowSurfacePtr windowSurface(new WindowSurface(pixelFormat));
    windowSurface->m_->surface = surface;

    return windowSurface;
}

WindowSurface::WindowSurface(DisplayPixelFormatPtr pixelFormat) :
    m_(new WindowSurfacePrivate(pixelFormat))
{
}

}
