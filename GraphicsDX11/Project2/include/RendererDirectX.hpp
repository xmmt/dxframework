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
    ~RendererDirectX() override = default;

private:
    RenderResult<void> init_() override;
    RenderResult<void> destroy_() override;
    RenderResult<void> prepareFrame_() const override;
    RenderResult<void> endFrame_() const override;
    RenderResult<void> draw_(
      Buffer<Vertex> const& vertices,
      Buffer<Color> const& colors,
      Buffer<int> const& indices,
      Material& material) const override;

    RenderResult<void> createContext_();
    RenderResult<void> createBackBuffer_();
    RenderResult<void> restoreTargets_() const;

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    ID3D11DeviceContext* context_{ nullptr };
    IDXGISwapChain* swapChain_{ nullptr };
    IDXGISwapChain1* swapChain1_{ nullptr };

    ID3D11Debug* debug_{ nullptr };
    ID3DUserDefinedAnnotation* annotation_{ nullptr };

    ID3D11RenderTargetView* renderView_;
    ID3D11Texture2D* backBuffer_;

    mutable std::chrono::time_point<std::chrono::steady_clock> prevTime_;
    int startTime_;
    mutable float totalTime_{ 0 };
    mutable unsigned int frameCount_{ 0 };
};

} // namespace GraphicsFramework