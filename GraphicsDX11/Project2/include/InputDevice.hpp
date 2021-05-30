#pragma once

#include <Utils.hpp>

namespace GraphicsFramework {

struct InputDeviceError {
    enum class Kind {
        UNDEFINED,
        BAD_CALLBACK,
    };

    Kind kind;
    std::string text;
};

template <typename T>
using InDeviceResult = Result<T, InputDeviceError>;

class InputDevice {
public:
    enum class Key : size_t {
        LEFT_MOUSE_BUTTON = 0,
        RIGHT_MOUSE_BUTTON,
        W,
        A,
        S,
        D,
        SPACEBAR,
        ESCAPE,
        ENTER,
        LEFT_SHIFT,
        LEFT_CTRL,
        _COUNT,
    };
    enum class EventType {
        PRESSED,
        RELEASED,
    };
    struct MousePosition {
        int x;
        int y;
    };

public:
    InDeviceResult<void> addOnKeyPressedCallback(Key key, std::function<void()> callback);
    InDeviceResult<void> addOnKeyReleasedCallback(Key key, std::function<void()> callback);
    InDeviceResult<void> addOnMouseMoveCallback(std::function<void(MousePosition)> callback);
    InDeviceResult<void> setKeyStateGetter(std::function<bool(Key)> callback);
    InDeviceResult<void> setMousePositionGetter(std::function<std::tuple<int, int>()> callback);

public:
    void onKeyPressed(Key key) const;
    void onKeyReleased(Key key) const;
    void onMouseMove(MousePosition position) const;

public:
    InDeviceResult<bool> isKeyPressed(Key key) const;
    InDeviceResult<std::tuple<int, int>> getMousePosition() const;

private:
    std::unordered_multimap<Key, std::function<void()>> onKeyPressedCallbacks_;
    std::unordered_multimap<Key, std::function<void()>> onKeyReleasedCallbacks_;
    std::function<void(MousePosition)> onMouseMove_;
    std::function<bool(Key)> keyStateGetter_;
    std::function<std::tuple<int, int>()> mousePositionGetter_;
};

} // namespace GraphicsFramework