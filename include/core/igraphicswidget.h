#ifndef CORE_IGRAPHICSWIDGET_H
#define CORE_IGRAPHICSWIDGET_H

#include <core/idevice.h>
#include <core/forwarddecl.h>

#include <utils/forwarddecl.h>
#include <utils/weakptrlist.h>
#include <utils/enumclass.h>
#include <utils/glm/vec2.hpp>

namespace simplex
{
namespace core
{
namespace graphics
{

ENUMCLASS(KeyState, uint16_t, Pressed, Released, Repeated)

ENUMCLASS(KeyCode, uint16_t,
    Space,
    Apostrophe,
    Comma,
    Minus,
    Period,
    Slash,
    Key_0,
    Key_1,
    Key_2,
    Key_3,
    Key_4,
    Key_5,
    Key_6,
    Key_7,
    Key_8,
    Key_9,
    Semicolon,
    Equal,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    LeftBracket,
    BackSlash,
    RightBracket,
    GraveQccent,
    World_1,
    World_2,
    Escape,
    Enter,
    Tab,
    Backspace,
    Insert,
    Delete,
    Right,
    Left,
    Down,
    Up,
    PageUp,
    PageDown,
    Home,
    End,
    CapsLock,
    ScrollLock,
    NumLock,
    PrintScreen,
    Pause,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    F25,
    Kp_0,
    Kp_1,
    Kp_2,
    Kp_3,
    Kp_4,
    Kp_5,
    Kp_6,
    Kp_7,
    Kp_8,
    Kp_9,
    KP_Decimal,
    KP_Divide,
    KP_Multiply,
    KP_Subtract,
    KP_Add,
    KP_Enter,
    KP_Equal,
    LeftShift,
    LeftControl,
    LeftAlt,
    LeftSuper,
    RightShift,
    RightControl,
    RightAlt,
    RightSuper,
    Menu)

ENUMCLASS(KeyModifier, uint16_t, Shift, Control, Alt, Super, CapsLock, NumLock)

ENUMCLASS(MouseButtonState, uint16_t, Pressed, Released)

ENUMCLASS(MouseButton, uint16_t, Left, Right, Middle)

ENUMCLASS(MouseStandardCursor, uint16_t, Arrow, IBeam, CrossHair, Hand, ResizeEW, ResizeNS, ResizeNWSE, ResizeNESW, ResizeAll, NotAllowed)

using ShareGroup = utils::WeakPtrList<IGraphicsWidget>;

class IGraphicsWidget : public IDevice
{
public:
    ~IGraphicsWidget() override = default;

    virtual std::shared_ptr<ShareGroup> shareGroup() = 0;
    virtual std::shared_ptr<const ShareGroup> shareGroup() const = 0;

    virtual std::shared_ptr<GraphicsEngine> graphicsEngine() = 0;
    virtual std::shared_ptr<const GraphicsEngine> graphicsEngine() const = 0;

    virtual std::shared_ptr<IFrameBuffer> defaultFrameBuffer() = 0;
    virtual std::shared_ptr<const IFrameBuffer> defaultFrameBuffer() const = 0;

    virtual std::string title() const = 0;
    virtual void setTitle(const std::string&) = 0;

    virtual void setIcon(const std::shared_ptr<utils::Image>&) = 0;

    virtual glm::uvec2 position() const = 0;
    virtual void setPosition(const glm::uvec2&) = 0;

    virtual glm::uvec2 size() const = 0;
    virtual void setSize(const glm::uvec2&) = 0;

    virtual void minimize() = 0;
    virtual void maximize() = 0;
    virtual void restore() = 0;

    virtual void show(bool = true) = 0;
    virtual void hide() = 0;

    virtual void setFocus() = 0;

    virtual glm::ivec2 mouseCursorPosition() const = 0;
    virtual void setMouseCursorPosition(const glm::ivec2&) = 0;

    virtual void showMouseCursor(bool = true) = 0;
    virtual void hideMouseCursor() = 0;

    virtual void setMouseStandardCursor(MouseStandardCursor) = 0;
    virtual void setMouseCursor(const std::shared_ptr<utils::Image>&, const glm::uvec2 &hotPoint) = 0;

    virtual bool isMouseCursorInside() const = 0;

    virtual KeyState keyState(KeyCode) const = 0;
    virtual std::string keyName(KeyCode) const = 0;
    virtual MouseButtonState mouseButtonState(MouseButton) const = 0;

    virtual CloseCallback closeCallback() const = 0;
    virtual void setCloseCallback(CloseCallback) = 0;

    virtual ResizeCallback resizeCallback() const = 0;
    virtual void setResizeCallback(ResizeCallback) = 0;

    virtual UpdateCallback updateCallback() const = 0;
    virtual void setUpdateCallback(UpdateCallback) = 0;

    virtual KeyCallback keyCallback() const = 0;
    virtual void setKeyCallback(KeyCallback) = 0;

    virtual MouseCursorMoveCallback mouseCursorMoveCallback() const = 0;
    virtual void setMouseCursorMoveCallback(MouseCursorMoveCallback) = 0;

    virtual MouseCursorEnterCallback mouseCursorEnterCallback() const = 0;
    virtual void setMouseCursorEnterCallback(MouseCursorEnterCallback) = 0;

    virtual MouseButtonCallback mouseButtonCallback() const = 0;
    virtual void setMouseButtonCallback(MouseButtonCallback) = 0;

    virtual MouseScrollCallback mouseScrollCallback() const = 0;
    virtual void setMouseScrollCallback(MouseScrollCallback) = 0;

};

}
}
}

#endif // CORE_IGRAPHICSWIDGET_H
