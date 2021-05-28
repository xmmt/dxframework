#pragma once

#include <Renderer.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinUser.h>
#include <wrl.h>

#include <d3d.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXColors.h>

#include <chrono>

namespace GraphicsFramework {

class RendererDirectX : public Renderer {
public:
    RendererDirectX(Display const& display);
    virtual ~RendererDirectX() = default;

public:
    //Result<void, RendererError> init() override;
    //Result<void, RendererError> destroy() override;
    //Result<void, RendererError> prepareFrame() override;
    //Result<void, RendererError> endFrame() override;
    //Result<void, RendererError> draw(Buffer<Vertex<4>> const& vertices, Buffer<Color<4>> const& colors, VertexShader& vertexShader, PixelShader& pixelShader) override;

private:
    Result<void, RendererError> init_() override;
    Result<void, RendererError> destroy_() override;
    Result<void, RendererError> prepareFrame_() override;
    Result<void, RendererError> endFrame_() override;
    Result<void, RendererError> draw_(Buffer<Vertex<4>> const& vertices, Buffer<Color<4>> const& colors, Buffer<int> const& indices, VertexShader& vertexShader, PixelShader& pixelShader) override;

    Result<void, RendererError> createContext_();
    Result<void, RendererError> createBackBuffer_();
    Result<void, RendererError> restoreTargets_();

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    ID3D11DeviceContext* context_{ nullptr };
    IDXGISwapChain* swapChain_{ nullptr };
    IDXGISwapChain1* swapChain1_{ nullptr };

    ID3D11Debug* debug_{ nullptr };
    ID3DUserDefinedAnnotation* annotation_{ nullptr };

    ID3D11RenderTargetView* renderView_;
    ID3D11Texture2D* backBuffer_;

    std::chrono::time_point<std::chrono::steady_clock> prevTime_;
    int startTime_;
    float totalTime_{ 0 };
    unsigned int frameCount_{ 0 };
};

} // namespace GraphicsFramework