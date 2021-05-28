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
    virtual Result<Size, DisplayError> size() const = 0;
    virtual Result<void, DisplayError> show() = 0;
    virtual Result<void, DisplayError> setTitle(std::string title) = 0;

public:
    virtual Result<void, DisplayError> addOnSizeUpdatedCallback(std::function<void(Size)> callback);

protected:
    void onSizeUpdated_(Size size) const;


protected:
    InputDevice& inputDevice_;
    std::list<std::function<void(Size)>> onSizeUpdatedCallbacks_;
};

} // namespace GraphicsFramework