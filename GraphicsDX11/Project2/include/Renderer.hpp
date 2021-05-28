#pragma once

#include <Display.hpp>
#include <Utils.hpp>
#include <Shader.hpp>

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
using Buffer = std::vector<T>;

template <size_t N>
using Vertex = std::array<float, N>;

template <size_t N>
using Color = std::array<float, N>;

class Renderer {
public:
    Renderer(Display const& display);

public:
    virtual Result<void, RendererError> init();
    virtual Result<void, RendererError> destroy();
    virtual Result<void, RendererError> prepareFrame();
    virtual Result<void, RendererError> endFrame();
    virtual Result<void, RendererError> draw(Buffer<Vertex<4>> const& vertices, Buffer<Color<4>> const& colors, Buffer<int> const& indices, VertexShader& vertexShader, PixelShader& pixelShader);

protected:
    virtual Result<void, RendererError> init_() = 0;
    virtual Result<void, RendererError> destroy_() = 0;
    virtual Result<void, RendererError> prepareFrame_() = 0;
    virtual Result<void, RendererError> endFrame_() = 0;
    virtual Result<void, RendererError> draw_(Buffer<Vertex<4>> const& vertices, Buffer<Color<4>> const& colors, Buffer<int> const& indices, VertexShader& vertexShader, PixelShader& pixelShader) = 0;

protected:
    bool initialized_{ false };
    bool frameStarted_{ false };
    Display const& display_;
    VertexShader* currentVertexShader{ nullptr };
    PixelShader* currentPixelShader{ nullptr };
    Buffer<Vertex<4>> vertices_;
    Buffer<Color<4>> colors_;
    Buffer<int> indices_;
};

} // namespace GraphicsFramework