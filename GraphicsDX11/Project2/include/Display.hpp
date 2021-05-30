#pragma once

#include <InputDevice.hpp>
#include <Utils.hpp>
#include <functional>

namespace GraphicsFramework {

struct DisplayError {
    enum class Kind {
        UNDEFINED,
        BAD_STATE,
    };

    Kind kind;
    std::string text;
};

template <typename T>
using DisplayResult = Result<T, DisplayError>;

class Display {
public:
    struct Size {
        std::uint32_t width;
        std::uint32_t height;
    };

public:
    Display(InputDevice& inputDevice);
    virtual ~Display() = default;

public:
    virtual DisplayResult<void> runLoop(std::function<void(float)> runFrame) const = 0;

public:
    virtual DisplayResult<Size> size() const = 0;
    virtual DisplayResult<void> show() = 0;
    virtual DisplayResult<void> setTitle(std::string title) = 0;

public:
    virtual Result<void, DisplayError> addOnSizeUpdatedCallback(std::function<void(Size)> callback);

protected:
    void onSizeUpdated_(Size size) const;


protected:
    InputDevice& inputDevice_;
    std::list<std::function<void(Size)>> onSizeUpdatedCallbacks_;
};

} // namespace GraphicsFramework