#pragma once

#include <dxframework/display.hpp>
#include <dxframework/utils.hpp>
#include <dxframework/shader.hpp>

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

namespace dxframework {

template <typename T>
using buffer = std::vector<T>;

// using Vertex = std::array<float, 4>;
using vertex = DirectX::XMFLOAT4;

// using Color = std::array<float, N>;
using color = DirectX::XMFLOAT4;

class renderer {
public:
    renderer(display& display);
    virtual ~renderer() = default;

public:
    void init();
    void destroy();
    void run_loop(std::function<void(float)> run_frame) const;
    void prepare_frame() const;
    void end_frame() const;
    void draw(
      buffer<vertex>& vertices,
      buffer<color>& colors,
      buffer<int>& indices) const;
    void set_shaders(vertex_shader& vs, pixel_shader& ps);
    void init_shaders(vertex_shader& vs, pixel_shader& ps);

private:
    void create_context_();
    void create_back_buffer_();
    void restore_targets_() const;

private:
    bool initialized_{ false };
    mutable bool frame_started_{ false };
    display const& display_;
    mutable buffer<vertex> vertices_;
    mutable buffer<color> colors_;
    mutable buffer<int> indices_;

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    ID3D11DeviceContext* context_{ nullptr };
    IDXGISwapChain* swap_chain_{ nullptr };
    IDXGISwapChain1* swap_chain1_{ nullptr };

    ID3D11Debug* debug_{ nullptr };
    ID3DUserDefinedAnnotation* annotation_{ nullptr };

    ID3D11RenderTargetView* render_view_;
    ID3D11Texture2D* back_buffer_;

    mutable std::chrono::time_point<std::chrono::steady_clock> prev_time_;
    int start_time_;
    mutable float total_time_{ 0 };
    mutable unsigned int frame_count_{ 0 };
};

} // namespace dxframework