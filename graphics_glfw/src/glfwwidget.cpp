#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <utils/logger.h>
#include <utils/image.h>

#include <core/graphicsengine.h>

#include <graphics_glfw/glfwwidget.h>

#include "glfwrenderer.h"
#include "glfwwidgetprivate.h"

namespace simplex
{
namespace graphics_glfw
{

static core::graphics::KeyState GLFWKeyStateToKeyState(int value)
{
    static std::unordered_map<int, core::graphics::KeyState> s_table{
        { GLFW_PRESS, core::graphics::KeyState::Pressed },
        { GLFW_RELEASE, core::graphics::KeyState::Released },
        { GLFW_REPEAT, core::graphics::KeyState::Repeated }
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? core::graphics::KeyState::Count : it->second;
}

static core::graphics::KeyCode GLFWKeyCodeToKeyCode(int value)
{
    static std::unordered_map<int, core::graphics::KeyCode> s_table{
        { GLFW_KEY_SPACE, core::graphics::KeyCode::Space },
        { GLFW_KEY_APOSTROPHE, core::graphics::KeyCode::Apostrophe },
        { GLFW_KEY_COMMA, core::graphics::KeyCode::Comma },
        { GLFW_KEY_MINUS, core::graphics::KeyCode::Minus },
        { GLFW_KEY_PERIOD, core::graphics::KeyCode::Period },
        { GLFW_KEY_SLASH, core::graphics::KeyCode::Slash },
        { GLFW_KEY_0, core::graphics::KeyCode::Key_0 },
        { GLFW_KEY_1, core::graphics::KeyCode::Key_1 },
        { GLFW_KEY_2, core::graphics::KeyCode::Key_2 },
        { GLFW_KEY_3, core::graphics::KeyCode::Key_3 },
        { GLFW_KEY_4, core::graphics::KeyCode::Key_4 },
        { GLFW_KEY_5, core::graphics::KeyCode::Key_5 },
        { GLFW_KEY_6, core::graphics::KeyCode::Key_6 },
        { GLFW_KEY_7, core::graphics::KeyCode::Key_7 },
        { GLFW_KEY_8, core::graphics::KeyCode::Key_8 },
        { GLFW_KEY_9, core::graphics::KeyCode::Key_9 },
        { GLFW_KEY_SEMICOLON, core::graphics::KeyCode::Semicolon },
        { GLFW_KEY_EQUAL, core::graphics::KeyCode::Equal },
        { GLFW_KEY_A, core::graphics::KeyCode::A },
        { GLFW_KEY_B, core::graphics::KeyCode::B },
        { GLFW_KEY_C, core::graphics::KeyCode::C },
        { GLFW_KEY_D, core::graphics::KeyCode::D },
        { GLFW_KEY_E, core::graphics::KeyCode::E },
        { GLFW_KEY_F, core::graphics::KeyCode::F },
        { GLFW_KEY_G, core::graphics::KeyCode::G },
        { GLFW_KEY_H, core::graphics::KeyCode::H },
        { GLFW_KEY_I, core::graphics::KeyCode::I },
        { GLFW_KEY_J, core::graphics::KeyCode::J },
        { GLFW_KEY_K, core::graphics::KeyCode::K },
        { GLFW_KEY_L, core::graphics::KeyCode::L },
        { GLFW_KEY_M, core::graphics::KeyCode::M },
        { GLFW_KEY_N, core::graphics::KeyCode::N },
        { GLFW_KEY_O, core::graphics::KeyCode::O },
        { GLFW_KEY_P, core::graphics::KeyCode::P },
        { GLFW_KEY_Q, core::graphics::KeyCode::Q },
        { GLFW_KEY_R, core::graphics::KeyCode::R },
        { GLFW_KEY_S, core::graphics::KeyCode::S },
        { GLFW_KEY_T, core::graphics::KeyCode::T },
        { GLFW_KEY_U, core::graphics::KeyCode::U },
        { GLFW_KEY_V, core::graphics::KeyCode::V },
        { GLFW_KEY_W, core::graphics::KeyCode::W },
        { GLFW_KEY_X, core::graphics::KeyCode::X },
        { GLFW_KEY_Y, core::graphics::KeyCode::Y },
        { GLFW_KEY_Z, core::graphics::KeyCode::Z },
        { GLFW_KEY_LEFT_BRACKET, core::graphics::KeyCode::LeftBracket },
        { GLFW_KEY_BACKSLASH, core::graphics::KeyCode::BackSlash },
        { GLFW_KEY_RIGHT_BRACKET, core::graphics::KeyCode::RightBracket },
        { GLFW_KEY_GRAVE_ACCENT, core::graphics::KeyCode::GraveQccent },
        { GLFW_KEY_WORLD_1, core::graphics::KeyCode::World_1 },
        { GLFW_KEY_WORLD_2, core::graphics::KeyCode::World_2 },
        { GLFW_KEY_ESCAPE, core::graphics::KeyCode::Escape },
        { GLFW_KEY_ENTER, core::graphics::KeyCode::Enter },
        { GLFW_KEY_TAB, core::graphics::KeyCode::Tab },
        { GLFW_KEY_BACKSPACE, core::graphics::KeyCode::Backspace },
        { GLFW_KEY_INSERT, core::graphics::KeyCode::Insert },
        { GLFW_KEY_DELETE, core::graphics::KeyCode::Delete },
        { GLFW_KEY_RIGHT, core::graphics::KeyCode::Right },
        { GLFW_KEY_LEFT, core::graphics::KeyCode::Left },
        { GLFW_KEY_DOWN, core::graphics::KeyCode::Down },
        { GLFW_KEY_UP, core::graphics::KeyCode::Up },
        { GLFW_KEY_PAGE_UP, core::graphics::KeyCode::PageUp },
        { GLFW_KEY_PAGE_DOWN, core::graphics::KeyCode::PageDown },
        { GLFW_KEY_HOME, core::graphics::KeyCode::Home },
        { GLFW_KEY_END, core::graphics::KeyCode::End },
        { GLFW_KEY_CAPS_LOCK, core::graphics::KeyCode::CapsLock },
        { GLFW_KEY_SCROLL_LOCK, core::graphics::KeyCode::ScrollLock },
        { GLFW_KEY_NUM_LOCK, core::graphics::KeyCode::NumLock },
        { GLFW_KEY_PRINT_SCREEN, core::graphics::KeyCode::PrintScreen },
        { GLFW_KEY_PAUSE, core::graphics::KeyCode::Pause },
        { GLFW_KEY_F1, core::graphics::KeyCode::F1 },
        { GLFW_KEY_F2, core::graphics::KeyCode::F2 },
        { GLFW_KEY_F3, core::graphics::KeyCode::F3 },
        { GLFW_KEY_F4, core::graphics::KeyCode::F4 },
        { GLFW_KEY_F5, core::graphics::KeyCode::F5 },
        { GLFW_KEY_F6, core::graphics::KeyCode::F6 },
        { GLFW_KEY_F7, core::graphics::KeyCode::F7 },
        { GLFW_KEY_F8, core::graphics::KeyCode::F8 },
        { GLFW_KEY_F9, core::graphics::KeyCode::F9 },
        { GLFW_KEY_F10, core::graphics::KeyCode::F10 },
        { GLFW_KEY_F11, core::graphics::KeyCode::F11 },
        { GLFW_KEY_F12, core::graphics::KeyCode::F12 },
        { GLFW_KEY_F13, core::graphics::KeyCode::F13 },
        { GLFW_KEY_F14, core::graphics::KeyCode::F14 },
        { GLFW_KEY_F15, core::graphics::KeyCode::F15 },
        { GLFW_KEY_F16, core::graphics::KeyCode::F16 },
        { GLFW_KEY_F17, core::graphics::KeyCode::F17 },
        { GLFW_KEY_F18, core::graphics::KeyCode::F18 },
        { GLFW_KEY_F19, core::graphics::KeyCode::F19 },
        { GLFW_KEY_F20, core::graphics::KeyCode::F20 },
        { GLFW_KEY_F21, core::graphics::KeyCode::F21 },
        { GLFW_KEY_F22, core::graphics::KeyCode::F22 },
        { GLFW_KEY_F23, core::graphics::KeyCode::F23 },
        { GLFW_KEY_F24, core::graphics::KeyCode::F24 },
        { GLFW_KEY_F25, core::graphics::KeyCode::F25 },
        { GLFW_KEY_KP_0, core::graphics::KeyCode::Kp_0 },
        { GLFW_KEY_KP_1, core::graphics::KeyCode::Kp_1 },
        { GLFW_KEY_KP_2, core::graphics::KeyCode::Kp_2 },
        { GLFW_KEY_KP_3, core::graphics::KeyCode::Kp_3 },
        { GLFW_KEY_KP_4, core::graphics::KeyCode::Kp_4 },
        { GLFW_KEY_KP_5, core::graphics::KeyCode::Kp_5 },
        { GLFW_KEY_KP_6, core::graphics::KeyCode::Kp_6 },
        { GLFW_KEY_KP_7, core::graphics::KeyCode::Kp_7 },
        { GLFW_KEY_KP_8, core::graphics::KeyCode::Kp_8 },
        { GLFW_KEY_KP_9, core::graphics::KeyCode::Kp_9 },
        { GLFW_KEY_KP_DECIMAL, core::graphics::KeyCode::KP_Decimal },
        { GLFW_KEY_KP_DIVIDE, core::graphics::KeyCode::KP_Divide },
        { GLFW_KEY_KP_MULTIPLY, core::graphics::KeyCode::KP_Multiply },
        { GLFW_KEY_KP_SUBTRACT, core::graphics::KeyCode::KP_Subtract },
        { GLFW_KEY_KP_ADD, core::graphics::KeyCode::KP_Add },
        { GLFW_KEY_KP_ENTER, core::graphics::KeyCode::KP_Enter },
        { GLFW_KEY_KP_EQUAL, core::graphics::KeyCode::KP_Equal },
        { GLFW_KEY_LEFT_SHIFT, core::graphics::KeyCode::LeftShift },
        { GLFW_KEY_LEFT_CONTROL, core::graphics::KeyCode::LeftControl },
        { GLFW_KEY_LEFT_ALT, core::graphics::KeyCode::LeftAlt },
        { GLFW_KEY_LEFT_SUPER, core::graphics::KeyCode::LeftSuper },
        { GLFW_KEY_RIGHT_SHIFT, core::graphics::KeyCode::RightShift },
        { GLFW_KEY_RIGHT_CONTROL, core::graphics::KeyCode::RightControl },
        { GLFW_KEY_RIGHT_ALT, core::graphics::KeyCode::RightAlt },
        { GLFW_KEY_RIGHT_SUPER, core::graphics::KeyCode::RightSuper },
        { GLFW_KEY_MENU, core::graphics::KeyCode::Menu }
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? core::graphics::KeyCode::Count : it->second;
}

static int KeyCodeToGLFWKeyCode(core::graphics::KeyCode value)
{
    static std::array<int, core::graphics::numElementsKeyCode()> s_table{
        GLFW_KEY_SPACE,
        GLFW_KEY_APOSTROPHE,
        GLFW_KEY_COMMA,
        GLFW_KEY_MINUS,
        GLFW_KEY_PERIOD,
        GLFW_KEY_SLASH,
        GLFW_KEY_0,
        GLFW_KEY_1,
        GLFW_KEY_2,
        GLFW_KEY_3,
        GLFW_KEY_4,
        GLFW_KEY_5,
        GLFW_KEY_6,
        GLFW_KEY_7,
        GLFW_KEY_8,
        GLFW_KEY_9,
        GLFW_KEY_SEMICOLON,
        GLFW_KEY_EQUAL,
        GLFW_KEY_A,
        GLFW_KEY_B,
        GLFW_KEY_C,
        GLFW_KEY_D,
        GLFW_KEY_E,
        GLFW_KEY_F,
        GLFW_KEY_G,
        GLFW_KEY_H,
        GLFW_KEY_I,
        GLFW_KEY_J,
        GLFW_KEY_K,
        GLFW_KEY_L,
        GLFW_KEY_M,
        GLFW_KEY_N,
        GLFW_KEY_O,
        GLFW_KEY_P,
        GLFW_KEY_Q,
        GLFW_KEY_R,
        GLFW_KEY_S,
        GLFW_KEY_T,
        GLFW_KEY_U,
        GLFW_KEY_V,
        GLFW_KEY_W,
        GLFW_KEY_X,
        GLFW_KEY_Y,
        GLFW_KEY_Z,
        GLFW_KEY_LEFT_BRACKET,
        GLFW_KEY_BACKSLASH,
        GLFW_KEY_RIGHT_BRACKET,
        GLFW_KEY_GRAVE_ACCENT,
        GLFW_KEY_WORLD_1,
        GLFW_KEY_WORLD_2,
        GLFW_KEY_ESCAPE,
        GLFW_KEY_ENTER,
        GLFW_KEY_TAB,
        GLFW_KEY_BACKSPACE,
        GLFW_KEY_INSERT,
        GLFW_KEY_DELETE,
        GLFW_KEY_RIGHT,
        GLFW_KEY_LEFT,
        GLFW_KEY_DOWN,
        GLFW_KEY_UP,
        GLFW_KEY_PAGE_UP,
        GLFW_KEY_PAGE_DOWN,
        GLFW_KEY_HOME,
        GLFW_KEY_END,
        GLFW_KEY_CAPS_LOCK,
        GLFW_KEY_SCROLL_LOCK,
        GLFW_KEY_NUM_LOCK,
        GLFW_KEY_PRINT_SCREEN,
        GLFW_KEY_PAUSE,
        GLFW_KEY_F1,
        GLFW_KEY_F2,
        GLFW_KEY_F3,
        GLFW_KEY_F4,
        GLFW_KEY_F5,
        GLFW_KEY_F6,
        GLFW_KEY_F7,
        GLFW_KEY_F8,
        GLFW_KEY_F9,
        GLFW_KEY_F10,
        GLFW_KEY_F11,
        GLFW_KEY_F12,
        GLFW_KEY_F13,
        GLFW_KEY_F14,
        GLFW_KEY_F15,
        GLFW_KEY_F16,
        GLFW_KEY_F17,
        GLFW_KEY_F18,
        GLFW_KEY_F19,
        GLFW_KEY_F20,
        GLFW_KEY_F21,
        GLFW_KEY_F22,
        GLFW_KEY_F23,
        GLFW_KEY_F24,
        GLFW_KEY_F25,
        GLFW_KEY_KP_0,
        GLFW_KEY_KP_1,
        GLFW_KEY_KP_2,
        GLFW_KEY_KP_3,
        GLFW_KEY_KP_4,
        GLFW_KEY_KP_5,
        GLFW_KEY_KP_6,
        GLFW_KEY_KP_7,
        GLFW_KEY_KP_8,
        GLFW_KEY_KP_9,
        GLFW_KEY_KP_DECIMAL,
        GLFW_KEY_KP_DIVIDE,
        GLFW_KEY_KP_MULTIPLY,
        GLFW_KEY_KP_SUBTRACT,
        GLFW_KEY_KP_ADD,
        GLFW_KEY_KP_ENTER,
        GLFW_KEY_KP_EQUAL,
        GLFW_KEY_LEFT_SHIFT,
        GLFW_KEY_LEFT_CONTROL,
        GLFW_KEY_LEFT_ALT,
        GLFW_KEY_LEFT_SUPER,
        GLFW_KEY_RIGHT_SHIFT,
        GLFW_KEY_RIGHT_CONTROL,
        GLFW_KEY_RIGHT_ALT,
        GLFW_KEY_RIGHT_SUPER,
        GLFW_KEY_MENU,

    };
    return s_table[core::graphics::castFromKeyCode(value)];
}

static core::graphics::KeyModifier GLFWKeyModifierToKeyModifier(int value)
{
    static std::unordered_map<int, core::graphics::KeyModifier> s_table{
        { GLFW_MOD_SHIFT, core::graphics::KeyModifier::Shift },
        { GLFW_MOD_CONTROL, core::graphics::KeyModifier::Control },
        { GLFW_MOD_ALT, core::graphics::KeyModifier::Alt },
        { GLFW_MOD_SUPER, core::graphics::KeyModifier::Super },
        { GLFW_MOD_CAPS_LOCK, core::graphics::KeyModifier::CapsLock },
        { GLFW_MOD_NUM_LOCK, core::graphics::KeyModifier::NumLock }
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? core::graphics::KeyModifier::Count : it->second;
}

static core::graphics::MouseButtonState GLFWMouseButonStateToMouseButonState(int value)
{
    static std::unordered_map<int, core::graphics::MouseButtonState> s_table{
        { GLFW_PRESS, core::graphics::MouseButtonState::Pressed },
        { GLFW_RELEASE, core::graphics::MouseButtonState::Released },
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? core::graphics::MouseButtonState::Count : it->second;
}

static core::graphics::MouseButton GLFWMouseButtonToMouseButton(int value)
{
    static std::unordered_map<int, core::graphics::MouseButton> s_table{
        { GLFW_MOUSE_BUTTON_LEFT, core::graphics::MouseButton::Left },
        { GLFW_MOUSE_BUTTON_RIGHT, core::graphics::MouseButton::Right },
        { GLFW_MOUSE_BUTTON_MIDDLE, core::graphics::MouseButton::Middle },
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? core::graphics::MouseButton::Count : it->second;
}

static int MouseButtonToGLFWMouseButton(core::graphics::MouseButton value)
{
    static std::array<int, core::graphics::numElementsKeyCode()> s_table{
        GLFW_MOUSE_BUTTON_LEFT,
        GLFW_MOUSE_BUTTON_RIGHT,
        GLFW_MOUSE_BUTTON_MIDDLE
    };
    return s_table[core::graphics::castFromMouseButton(value)];
}

static int MouseStandardCursorToGLFWMouseStandardCursor(core::graphics::MouseStandardCursor value)
{
    static std::array<int, core::graphics::numElementsMouseStandardCursor()> s_table{
        GLFW_ARROW_CURSOR,
        GLFW_IBEAM_CURSOR,
        GLFW_CROSSHAIR_CURSOR,
        GLFW_HAND_CURSOR,
        GLFW_RESIZE_EW_CURSOR,
        GLFW_RESIZE_NS_CURSOR,
        GLFW_RESIZE_NWSE_CURSOR,
        GLFW_RESIZE_NESW_CURSOR,
        GLFW_RESIZE_ALL_CURSOR,
        GLFW_NOT_ALLOWED_CURSOR
    };
    return s_table[core::graphics::castFromMouseStandardCursor(value)];
}

static void closeWidget(const GLFWWidget& widget)
{
    if (!widget.isInitialized())
        return;

    if (auto callback = widget.closeCallback())
        callback();

    auto& widgetPrivate = widget.m();

    if (auto& graphicsEngine = widgetPrivate.engine())
        if (auto graphicsRenderer = graphicsEngine->graphicsRenderer())
        {
            graphicsRenderer->makeCurrent();
            widgetPrivate.scene() = nullptr;
            widgetPrivate.defaultFrameBuffer() = nullptr;
            widgetPrivate.engine() = nullptr;
        }

    if (auto& window = widgetPrivate.window())
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    if (auto& cursor = widgetPrivate.cursor())
    {
        glfwDestroyCursor(cursor);
        cursor = nullptr;
    }
}

static std::shared_ptr<GLFWWidget> findGLFWWidget(GLFWwindow* window)
{
    std::shared_ptr<GLFWWidget> result;
    for (const auto& widgetWeak : GLFWWidgetPrivate::instances())
    {
        if (widgetWeak.expired())
            continue;

        auto widget = widgetWeak.lock();
        if (widget->m().window() == window)
        {
            result = widget;
            break;
        }
    }

    return result;
}

static void GLFWErrorCallback(int error, const char* description)
{
    LOG_ERROR << description;
}

static void GLFWCloseCallback(GLFWwindow *window)
{
    auto widget = findGLFWWidget(window);
    if (widget)
        closeWidget(*widget);
}

static void GLFWResizeCallback(GLFWwindow* window, int width, int height)
{
    auto widget = findGLFWWidget(window);
    if (!widget)
        return;

    const glm::uvec2 size(width, height);

    auto& widgetPrivate = widget->m();

    if (auto& graphicsEngine = widgetPrivate.engine())
        if (auto graphicsRenderer = graphicsEngine->graphicsRenderer())
        {
            graphicsRenderer->makeCurrent();
            //glfwGetFramebufferSize(window, &width, &height);
            graphicsRenderer->resize(size);
        }

    if (auto callback = widget->resizeCallback())
        callback(size);

    LOG_INFO << "GLFWWidget has been resized (" << width << ", " << height << ")";
}

static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto widget = findGLFWWidget(window);
    if (!widget)
        return;

    auto keyState = GLFWKeyStateToKeyState(action);
    if (keyState == core::graphics::KeyState::Count)
    {
        LOG_ERROR << "Undefined key state";
        return;
    }

    auto keyCode = GLFWKeyCodeToKeyCode(key);
    if (keyCode == core::graphics::KeyCode::Count)
    {
        LOG_ERROR << "Undefined key code";
        return;
    }

    core::graphics::KeyModifiers keyModifiers;
    for (auto mod : {GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER, GLFW_MOD_CAPS_LOCK, GLFW_MOD_NUM_LOCK})
    {
        if (!(mods & mod))
            continue;

        auto keyModifier = GLFWKeyModifierToKeyModifier(mod);
        if (keyModifier == core::graphics::KeyModifier::Count)
        {
            LOG_WARNING << "Undefined key modifier";
            continue;
        }

        keyModifiers.insert(keyModifier);
    }

    if (auto callback = widget->keyCallback())
        callback(keyState, keyCode, keyModifiers);
}

static void GLFWMouseCursorMoveCallback(GLFWwindow* window, double x, double y)
{
    auto widget = findGLFWWidget(window);
    if (!widget)
        return;

    if (auto callback = widget->mouseCursorMoveCallback())
        callback(glm::ivec2(x, y));
}

static void GLFWMouseCursorEnterCallback(GLFWwindow* window, int isEntered)
{
    auto widget = findGLFWWidget(window);
    if (!widget)
        return;

    if (auto callback = widget->mouseCursorEnterCallback())
        callback(isEntered);
}

static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto widget = findGLFWWidget(window);
    if (!widget)
        return;

    auto mouseButtonState = GLFWMouseButonStateToMouseButonState(action);
    if (mouseButtonState == core::graphics::MouseButtonState::Count)
    {
        LOG_ERROR << "Undefined mouse button state";
        return;
    }

    auto mouseButton = GLFWMouseButtonToMouseButton(button);
    if (mouseButton == core::graphics::MouseButton::Count)
    {
        LOG_ERROR << "Undefined mouse button";
        return;
    }

    core::graphics::KeyModifiers keyModifiers;
    for (auto mod : { GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER, GLFW_MOD_CAPS_LOCK, GLFW_MOD_NUM_LOCK })
    {
        if (!(mods & mod))
            continue;

        auto keyModifier = GLFWKeyModifierToKeyModifier(mod);
        if (keyModifier == core::graphics::KeyModifier::Count)
        {
            LOG_WARNING << "Undefined key modifier";
            continue;
        }

        keyModifiers.insert(keyModifier);
    }

    if (auto callback = widget->mouseButtonCallback())
        callback(mouseButtonState, mouseButton, keyModifiers);
}

static void GLFWMouseScrollCallback(GLFWwindow* window, double x, double y)
{
    auto widget = findGLFWWidget(window);
    if (!widget)
        return;

    if (auto callback = widget->mouseScrollCallback())
        callback(glm::ivec2(x, y));
}

GLFWWidget::~GLFWWidget()
{
    closeWidget(*this);
    LOG_INFO << "GLFWWidget \"" << GLFWWidget::name() << "\" has been destroyed";
}

const std::string & GLFWWidget::name() const
{
    return m_->name();
}

bool GLFWWidget::isInitialized() const
{
    return m_->window() != nullptr;
}

void GLFWWidget::update(uint64_t time, uint32_t dt, core::debug::SceneInformation& sceneInfo)
{
    if (auto &callback = m_->updateCallback())
        callback(time, dt);

    if (auto &scene = m_->scene())
        m_->engine()->update(scene, time, dt, sceneInfo);

    glfwSwapBuffers(m_->window());
}

std::shared_ptr<core::IEngine> GLFWWidget::engine()
{
    return graphicsEngine();
}

std::shared_ptr<const core::IEngine> GLFWWidget::engine() const
{
    return graphicsEngine();
}

std::shared_ptr<core::Scene> GLFWWidget::scene()
{
    return m_->scene();
}

std::shared_ptr<const core::Scene> GLFWWidget::scene() const
{
    return const_cast<GLFWWidget*>(this)->scene();
}

void GLFWWidget::setScene(const std::shared_ptr<core::Scene> &value)
{
    m_->scene() = value;
}

std::shared_ptr<core::GraphicsEngine> GLFWWidget::graphicsEngine()
{
    return m_->engine();
}

std::shared_ptr<const core::GraphicsEngine> GLFWWidget::graphicsEngine() const
{
    return const_cast<GLFWWidget*>(this)->graphicsEngine();
}

std::shared_ptr<core::graphics::IFrameBuffer> GLFWWidget::defaultFrameBuffer()
{
    return m().defaultFrameBuffer();
}

std::shared_ptr<const core::graphics::IFrameBuffer> GLFWWidget::defaultFrameBuffer() const
{
    return const_cast<GLFWWidget*>(this)->defaultFrameBuffer();
}

std::string GLFWWidget::title() const
{
    return glfwGetWindowTitle(m_->window());
}

void GLFWWidget::setTitle(const std::string &value)
{
    glfwSetWindowTitle(m_->window(), value.c_str());
}

void GLFWWidget::setIcon(const std::shared_ptr<utils::Image> &image)
{
    std::shared_ptr<GLFWimage> icon;
    std::shared_ptr<utils::Image> imageRGBA8;
    
    if (image)
    {
        imageRGBA8 = image->toRGBA8();
        icon = std::make_shared<GLFWimage>();
        icon->width = static_cast<int>(imageRGBA8->width());
        icon->height = static_cast<int>(imageRGBA8->height());
        icon->pixels = imageRGBA8->data();
    }

    glfwSetWindowIcon(m_->window(), icon ? 1 : 0, icon.get());
}

glm::uvec2 GLFWWidget::position() const
{
    int x = 0;
    int y = 0;
    glfwGetWindowPos(m_->window(), &x, &y);

    return glm::uvec2(x, y);
}

void GLFWWidget::setPosition(const glm::uvec2 &value)
{
    glfwSetWindowPos(m_->window(), static_cast<int>(value.x), static_cast<int>(value.y));
}

glm::uvec2 GLFWWidget::size() const
{
    int w = 0;
    int h = 0;
    glfwGetFramebufferSize(m_->window(), &w, &h);

    return glm::uvec2(w, h);
}

void GLFWWidget::setSize(const glm::uvec2& value)
{
    glfwSetWindowSize(m_->window(), static_cast<int>(value.x), static_cast<int>(value.y));
}

void GLFWWidget::minimize()
{
    glfwIconifyWindow(m_->window());
}

void GLFWWidget::maximize()
{
    glfwMaximizeWindow(m_->window());
}

void GLFWWidget::restore()
{
    glfwRestoreWindow(m_->window());
}

void GLFWWidget::show(bool value)
{
    if (!value)
        return hide();

    glfwShowWindow(m_->window());
}

void GLFWWidget::hide()
{
    glfwHideWindow(m_->window());
}

void GLFWWidget::setFocus()
{
    glfwFocusWindow(m_->window());
}

glm::ivec2 GLFWWidget::mouseCursorPosition() const
{
    double x, y;
    glfwGetCursorPos(m_->window(), &x, &y);
    return glm::uvec2(x, y);
}

void GLFWWidget::setMouseCursorPosition(const glm::ivec2& value)
{
    glfwSetCursorPos(m_->window(), value.x, value.y);
}

void GLFWWidget::showMouseCursor(bool value)
{
    if (!value)
        hideMouseCursor();

    glfwSetInputMode(m_->window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void GLFWWidget::hideMouseCursor()
{
    glfwSetInputMode(m_->window(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void GLFWWidget::setMouseStandardCursor(core::graphics::MouseStandardCursor value)
{
    glfwDestroyCursor(m_->cursor());
    m_->cursor() = glfwCreateStandardCursor(MouseStandardCursorToGLFWMouseStandardCursor(value));
    glfwSetCursor(m_->window(), m_->cursor());
}

void GLFWWidget::setMouseCursor(const std::shared_ptr<utils::Image> &image, const glm::uvec2& hotPoint)
{
    if (!image)
    {
        setMouseStandardCursor(core::graphics::MouseStandardCursor::Arrow);
        return;
    }

    auto imageRGBA8 = image->toRGBA8();

    auto cursorImage = std::make_shared<GLFWimage>();
    cursorImage->width = static_cast<int>(imageRGBA8->width());
    cursorImage->height = static_cast<int>(imageRGBA8->height());
    cursorImage->pixels = imageRGBA8->data();

    glfwDestroyCursor(m_->cursor());
    m_->cursor() = glfwCreateCursor(cursorImage.get(), static_cast<int>(hotPoint.x), static_cast<int>(hotPoint.y));
    glfwSetCursor(m_->window(), m_->cursor());
}

bool GLFWWidget::isMouseCursorInside() const
{
    return glfwGetWindowAttrib(m_->window(), GLFW_HOVERED) == GLFW_TRUE;
}

core::graphics::KeyState GLFWWidget::keyState(core::graphics::KeyCode value) const
{
    return glfwGetKey(m_->window(), KeyCodeToGLFWKeyCode(value)) == GLFW_PRESS ?
        core::graphics::KeyState::Pressed : core::graphics::KeyState::Released;
}

std::string GLFWWidget::keyName(core::graphics::KeyCode value) const
{
    return glfwGetKeyName(KeyCodeToGLFWKeyCode(value), 0);
}

core::graphics::MouseButtonState GLFWWidget::mouseButtonState(core::graphics::MouseButton value) const
{
    return glfwGetMouseButton(m_->window(), MouseButtonToGLFWMouseButton(value)) == GLFW_PRESS ?
        core::graphics::MouseButtonState::Pressed : core::graphics::MouseButtonState::Released;
}

core::graphics::CloseCallback GLFWWidget::closeCallback() const
{
    return m_->closeCallback();
}

void GLFWWidget::setCloseCallback(core::graphics::CloseCallback value)
{
    m_->closeCallback() = value;
}

core::graphics::ResizeCallback GLFWWidget::resizeCallback() const
{
    return m_->resizeCallback();
}

void GLFWWidget::setResizeCallback(core::graphics::ResizeCallback value)
{
    m_->resizeCallback() = value;
}

core::graphics::UpdateCallback GLFWWidget::updateCallback() const
{
    return m_->updateCallback();
}

void GLFWWidget::setUpdateCallback(core::graphics::UpdateCallback value)
{
    m_->updateCallback() = value;
}

core::graphics::KeyCallback GLFWWidget::keyCallback() const
{
    return m_->keyCallback();
}

void GLFWWidget::setKeyCallback(core::graphics::KeyCallback value)
{
    m_->keyCallback() = value;
}

core::graphics::MouseCursorMoveCallback GLFWWidget::mouseCursorMoveCallback() const
{
    return m_->mouseCursorMoveCallback();
}

void GLFWWidget::setMouseCursorMoveCallback(core::graphics::MouseCursorMoveCallback value)
{
    m_->mouseCursorMoveCallback() = value;
}

core::graphics::MouseCursorEnterCallback GLFWWidget::mouseCursorEnterCallback() const
{
    return m_->mouseCursorEnterCallback();
}

void GLFWWidget::setMouseCursorEnterCallback(core::graphics::MouseCursorEnterCallback value)
{
    m_->mouseCursorEnterCallback() = value;
}

core::graphics::MouseButtonCallback GLFWWidget::mouseButtonCallback() const
{
    return m_->mouseButtonCallback();
}

void GLFWWidget::setMouseButtonCallback(core::graphics::MouseButtonCallback value)
{
    m_->mouseButtonCallback() = value;
}

core::graphics::MouseScrollCallback GLFWWidget::mouseScrollCallback() const
{
    return m_->mouseScrollCallback();
}

void GLFWWidget::setMouseScrollCallback(core::graphics::MouseScrollCallback value)
{
    m_->mouseScrollCallback() = value;
}

std::shared_ptr<core::graphics::ShareGroup> GLFWWidget::shareGroup()
{
    return m_->shareGroup();
}

std::shared_ptr<const core::graphics::ShareGroup> GLFWWidget::shareGroup() const
{
    return const_cast<GLFWWidget*>(this)->shareGroup();
}

std::shared_ptr<GLFWWidget> GLFWWidget::getOrCreate(const std::string &name, const std::shared_ptr<GLFWWidget> &sharedWidget)
{
    std::shared_ptr<GLFWWidget> result;

    auto &instances = GLFWWidgetPrivate::instances();
    if (auto it = instances.find_if([&name](const std::shared_ptr<GLFWWidget>& value) { return value->name() == name; });
        it != instances.end())
    {
        result = it->lock();
    }

    if (!result)
    {
        std::shared_ptr<core::graphics::ShareGroup> shareGroup;
        if (sharedWidget)
            shareGroup = sharedWidget->shareGroup();

        if (!shareGroup)
            shareGroup = std::make_shared<core::graphics::ShareGroup>();

        result = std::shared_ptr<GLFWWidget>(new GLFWWidget(name, shareGroup));
        auto& resultPrivate = result->m();

        auto renderer = std::make_shared<GLFWRenderer>(name + "Renderer", result);
        renderer->makeCurrent();
        renderer->resize(result->size());

        auto engine = std::make_shared<core::GraphicsEngine>(name + "Engine", renderer);
        resultPrivate.engine() = engine;

        resultPrivate.defaultFrameBuffer() = DefaultFrameBuffer_4_5::create(0u);

        shareGroup->push_back(result);
        instances.push_back(result);

        LOG_INFO << "GLFWWidget has been created (Name: \"" << name <<
            "\", OpenGL context ver:" << GLFWWidgetPrivate::majorVersion() << "." <<
            GLFWWidgetPrivate::minorVersion() << ")";
    }

    return result;
}

void GLFWWidget::pollEvents()
{
    glfwPollEvents();
}

uint64_t GLFWWidget::time()
{
    return static_cast<uint64_t>(glfwGetTime() * 1000.);
}

//void GLFWWidget::run()
//{
//    while (true)
//    {
//        bool shouldClose = true;
//
//        for (const auto& widgetWeak : GLFWWidgetPrivate::instances())
//        {
//            if (widgetWeak.expired())
//                continue;
//
//            auto widget = widgetWeak.lock();
//
//            widget->graphicsEngine()->graphicsRenderer()->makeCurrent();
//            glClearColor(.5f, .5f, 1.f, 1.f);
//            glClear(GL_COLOR_BUFFER_BIT);
//
//            widget->swapBuffers();
//
//            shouldClose = false;
//        }
//
//        if (shouldClose)
//            break;
//
//        glfwPollEvents();
//    }
//
//    glfwTerminate();
//}

//void QtOpenGLWidget::resizeGL(int width, int height)
//{
//    if (auto pd = dynamic_cast<QOpenGLPaintDevice*>(redirected(nullptr)); pd)
//    {
//        width = pd->width();
//        height = pd->height();
//    }
//
//    LOG_INFO << "QtOpenGLWidget::resizeGL(" << width << ", " << height << ")";
//    if (auto &renderer = m_->renderer(); renderer)
//        renderer->resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
//}
//
//void QtOpenGLWidget::paintGL()
//{
//    if (!m_->renderer())
//    {
//        m_->renderer() = QtOpenGL_4_5_Renderer::create(context());
//        m_->defaultFrameBuffer() = std::make_shared<DefaultFrameBuffer_4_5>(defaultFramebufferObject());
//
//        m_->startTime() = m_->lastFpsTime() = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch());
//        m_->lastUpdateTime() = 0u;
//    }
//
//    auto time = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch()) - m_->startTime();
//    auto dt = static_cast<uint32_t>(time - m_->lastUpdateTime());
//    m_->lastUpdateTime() = time;
//
//    static const uint64_t deltaFps = 1000u;
//    ++m_->fpsCounter();
//    if (time - m_->lastFpsTime() >= deltaFps)
//    {
//        auto lastFps = static_cast<uint32_t>(static_cast<float>(m_->fpsCounter()) / (0.001f * deltaFps) + 0.5f);
//        m_->fpsCounter() = 0u;
//        m_->lastFpsTime() = time;
//
//        //LOG_INFO << "FPS: " << lastFps;
//    }
//
//    if (auto app = application())
//        app->update(shared_from_this(), time, dt);
//}

GLFWWidget::GLFWWidget(const std::string &name, const std::shared_ptr<core::graphics::ShareGroup> &shareGroup)
    : m_(std::make_unique<GLFWWidgetPrivate>(name, shareGroup))
{
    if (!GLFWWidgetPrivate::isGLFWInitialized())
    {
        glfwSetErrorCallback(GLFWErrorCallback);

        if (!glfwInit())
            LOG_CRITICAL << "Failed to initialize GLFW";

        auto tempWindow = glfwCreateWindow(640, 480, name.c_str(), nullptr, nullptr);
        if (!tempWindow)
            LOG_CRITICAL << "Failed to create GLFW window " << name;

        glfwMakeContextCurrent(tempWindow);
        gladLoadGL(glfwGetProcAddress);

        auto version = glad_gl_find_core_gl();
        GLFWWidgetPrivate::majorVersion() = GLAD_VERSION_MAJOR(version);
        GLFWWidgetPrivate::minorVersion() = GLAD_VERSION_MINOR(version);

        glfwMakeContextCurrent(nullptr);
        glfwDestroyWindow(tempWindow);

        GLFWWidgetPrivate::isGLFWInitialized() = true;
    }

    if (!shareGroup)
        LOG_CRITICAL << "Share group can't be nullptr";

    GLFWwindow* shareWindow = nullptr;
    if (auto it = shareGroup->find_any(); it != shareGroup->end())
    {
        auto widget = std::dynamic_pointer_cast<GLFWWidget>(it->lock());
        if (!widget)
            LOG_CRITICAL << "Device \"" << it->lock()->name() << "\" can't be casted to GLFWWidget";
        
        shareWindow = widget->m().window();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLFWWidgetPrivate::majorVersion());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLFWWidgetPrivate::minorVersion());
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    static const uint32_t width = 640u;
    static const uint32_t height = 480u;

    auto window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), name.c_str(), nullptr, shareWindow);
    if (!window)
        LOG_CRITICAL << "Failed to create GLFW window " << name;

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    glfwSetWindowCloseCallback(window, GLFWCloseCallback);
    glfwSetFramebufferSizeCallback(window, GLFWResizeCallback);
    glfwSetKeyCallback(window, GLFWKeyCallback);
    glfwSetCursorPosCallback(window, GLFWMouseCursorMoveCallback);
    glfwSetCursorEnterCallback(window, GLFWMouseCursorEnterCallback);
    glfwSetMouseButtonCallback(window, GLFWMouseButtonCallback);
    glfwSetScrollCallback(window, GLFWMouseScrollCallback);

    m_->window() = window;
}

}
}
