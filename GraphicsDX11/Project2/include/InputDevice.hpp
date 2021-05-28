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
    Result<void, InputDeviceError> addOnKeyPressedCallback(Key key, std::function<void()> callback);
    Result<void, InputDeviceError> addOnKeyReleasedCallback(Key key, std::function<void()> callback);
    Result<void, InputDeviceError> addOnMouseMoveCallback(std::function<void(MousePosition)> callback);
    Result<void, InputDeviceError> setKeyStateGetter(std::function<bool(Key)> callback);
    Result<void, InputDeviceError> setMousePositionGetter(std::function<std::tuple<int, int>()> callback);

public:
    void onKeyPressed(Key key) const;
    void onKeyReleased(Key key) const;
    void onMouseMove(MousePosition position) const;

public:
    Result<bool, InputDeviceError> isKeyPressed(Key key) const;
    Result<std::tuple<int, int>, InputDeviceError> getMousePosition() const;

private:
    std::unordered_multimap<Key, std::function<void()>> onKeyPressedCallbacks_;
    std::unordered_multimap<Key, std::function<void()>> onKeyReleasedCallbacks_;
    std::function<void(MousePosition)> onMouseMove_;
    std::function<bool(Key)> keyStateGetter_;
    std::function<std::tuple<int, int>()> mousePositionGetter_;
};

} // namespace GraphicsFramework