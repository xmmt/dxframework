#include <InputDevice.hpp>

using namespace GraphicsFramework;

Result<void, InputDeviceError>
InputDevice::addOnKeyPressedCallback(Key key, std::function<void()> callback) {
    onKeyPressedCallbacks_.emplace(key, std::move(callback));
    return Ok();
}

Result<void, InputDeviceError>
InputDevice::addOnKeyReleasedCallback(Key key, std::function<void()> callback) {
    onKeyReleasedCallbacks_.emplace(key, std::move(callback));
    return Ok();
}

Result<void, InputDeviceError>
InputDevice::addOnMouseMoveCallback(std::function<void(MousePosition)> callback) {
    onMouseMove_ = std::move(callback);
    return Ok();
}

Result<void, InputDeviceError>
InputDevice::setKeyStateGetter(std::function<bool(Key)> callback) {
    keyStateGetter_ = std::move(callback);
    return Ok();
}

Result<void, InputDeviceError>
InputDevice::setMousePositionGetter(std::function<std::tuple<int, int>()> callback) {
    mousePositionGetter_ = std::move(callback);
    return Ok();
}

void InputDevice::onKeyPressed(Key key) const {
    auto r = onKeyPressedCallbacks_.equal_range(key);
    for (auto it = r.first; it != r.second; std::advance(it, 1)) {
        it->second();
    }
}

void InputDevice::onKeyReleased(Key key) const {
    auto r = onKeyReleasedCallbacks_.equal_range(key);
    for (auto it = r.first; it != r.second; std::advance(it, 1)) {
        it->second();
    }
}

void InputDevice::onMouseMove(MousePosition position) const {
    if (onMouseMove_) {
        onMouseMove_(std::move(position));
    }
}

Result<bool, InputDeviceError> InputDevice::isKeyPressed(Key key) const {
    if (keyStateGetter_) {
        return Ok(keyStateGetter_(key));
    }
    return Err(InputDeviceError{
      .kind = InputDeviceError ::Kind::BAD_CALLBACK,
      .text = "onKeyState callback is not set" });
}

Result<std::tuple<int, int>, InputDeviceError> InputDevice::getMousePosition() const {
    if (mousePositionGetter_) {
        return Ok(mousePositionGetter_());
    }
    return Err(InputDeviceError{
      .kind = InputDeviceError ::Kind::BAD_CALLBACK,
      .text = "onMousePosition callback is not set" });
}