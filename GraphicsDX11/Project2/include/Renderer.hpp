#pragma once

#include <Display.hpp>
#include <Utils.hpp>
#include <Material.hpp>

#include <DirectXMath.h>

namespace GraphicsFramework {

struct RendererError {
    enum class Kind {
        UNDEFINED,
        BAD_STATE,
        INIT_FAILED,
        DRAW_ERROR,
    };

    Kind kind;
    std::string text;

    static auto fromDisplayError(DisplayError&& err) {
        return RendererError{
            .kind = err.kind == DisplayError::Kind::BAD_STATE
              ? Kind::BAD_STATE
              : Kind::UNDEFINED,
            .text = std::move(err.text)
        };
    }
};

template <typename T>
using RenderResult = Result<T, RendererError>;

template <typename T>
using Buffer = std::vector<T>;

// using Vertex = std::array<float, 4>;
using Vertex = DirectX::XMFLOAT4;

// using Color = std::array<float, N>;
using Color = DirectX::XMFLOAT4;

class Renderer {
public:
    Renderer(Display const& display);
    virtual ~Renderer() = default;

public:
    virtual RenderResult<void> init();
    virtual RenderResult<void> destroy();
    virtual RenderResult<void> runLoop(std::function<void(float)> runFrame) const;
    virtual RenderResult<void> prepareFrame() const;
    virtual RenderResult<void> endFrame() const;
    virtual RenderResult<void> draw(
      Buffer<Vertex> const& vertices,
      Buffer<Color> const& colors,
      Buffer<int> const& indices,
      Material& material) const;

protected:
    virtual RenderResult<void> init_() = 0;
    virtual RenderResult<void> destroy_() = 0;
    virtual RenderResult<void> prepareFrame_() const = 0;
    virtual RenderResult<void> endFrame_() const = 0;
    virtual RenderResult<void> draw_(
      Buffer<Vertex> const& vertices,
      Buffer<Color> const& colors,
      Buffer<int> const& indices,
      Material& material) const = 0;

protected:
    bool initialized_{ false };
    mutable bool frameStarted_{ false };
    Display const& display_;
    Material* currentMaterial_{ nullptr };
    mutable Buffer<Vertex> vertices_;
    mutable Buffer<Color> colors_;
    mutable Buffer<int> indices_;
};

} // namespace GraphicsFramework