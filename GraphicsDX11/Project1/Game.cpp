#pragma once

#include "pch.h"

#include "Game.hpp"

#include <future>

using namespace DirectX;

void Game::initialize() {
    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = 2;
    swapDesc.BufferDesc.Width = display_.width();
    swapDesc.BufferDesc.Height = display_.height();
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = display_.hWnd();
    swapDesc.Windowed = true;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;

    D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_0 };
    ZCHECK(D3D11CreateDeviceAndSwapChain(
      nullptr,
      D3D_DRIVER_TYPE_HARDWARE,
      nullptr,
      D3D11_CREATE_DEVICE_DEBUG,
      featureLevel,
      1,
      D3D11_SDK_VERSION,
      &swapDesc,
      &swapChain_,
      &device_,
      nullptr,
      &context_));

    ZCHECK(swapChain_->QueryInterface<IDXGISwapChain1>(&swapChain1_));
    ZCHECK(context_->QueryInterface<ID3DUserDefinedAnnotation>(&annotation_));
    ZCHECK(device_->QueryInterface<ID3D11Debug>(&debug_));

    //ID3D11Device5* device1 = nullptr;
    //device->QueryInterface(IID_ID3D11Device5, (void**)&device1);
    //
    //IDXGIDevice4* dxgiDevice4;
    //device1->QueryInterface(IID_IDXGIDevice4, (void**)&dxgiDevice4);
    ////context->QueryInterface(IID_ID3D11DeviceContext1);
    //IDXGIAdapter* adapter = nullptr;
    //dxgiDevice4->GetAdapter(&adapter);
    //
    //IDXGIOutput* output = nullptr;
    //adapter->EnumOutputs(0, &output);
    //DXGI_OUTPUT_DESC outputDesc{};
    //output->GetDesc(&outputDesc);
    //
    //DXGI_ADAPTER_DESC adapterDesc{};
    //adapter->GetDesc(&adapterDesc);
    //
    //
    //IDXGIFactory5* factory = nullptr;
    //adapter->GetParent(IID_IDXGIFactory5, (void**)&factory);
}

void Game::createBackBuffer() {
    ZCHECK(swapChain_->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBuffer_));
    ZCHECK(device_->CreateRenderTargetView(backBuffer_, nullptr, &renderView_));
}

void Game::prepareResources() {
    for (auto&& i : components_) {
        i->initialize(device_.Get());
    }
}

Game::Game(DisplayWin32& display, InputDevice& inputDevice, std::vector<std::shared_ptr<GameComponent>> components)
    : display_{ display }
    , inputDevice_{ inputDevice }
    , components_{ std::move(components) } {

    initialize();
    createBackBuffer();
    prepareResources();
}

LRESULT CALLBACK Game::messageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
    //auto id = std::this_thread::get_id();
    //WriteInfo() << "WndProc id: " << id << "\n";

    switch (umessage) {
        // Check if the window is being destroyed.
    case WM_DESTROY:
    case WM_CLOSE: {
        PostQuitMessage(0);
        return 0;
    }

    case WM_SIZE: {
        WriteInfo() << "Width " << LOWORD(lparam) << " Height " << HIWORD(lparam) << "\n";
        return 0;
    }

    //case WM_WINDOWPOSCHANGED: {
    //    return 0;
    //}

    // Check if a key has been pressed on the keyboard.
    case WM_KEYDOWN: {
        // If a key is pressed send it to the input object so it can record that state.
        WriteInfo() << "Key: " << static_cast<unsigned int>(wparam) << "\n";

        if (static_cast<unsigned int>(wparam) == 27) {
            PostQuitMessage(0);
        }
        return 0;
    }

    // Check if a key has been released on the keyboard.
    case WM_KEYUP: {
        // If a key is released then send it to the input object so it can unset the state for that key.
        return 0;
    }

    // All other messages pass to the message handler in the system class.
    default: {
        return DefWindowProc(hwnd, umessage, wparam, lparam);
    }
    }
}

void Game::run() {
    auto renderThread = std::async(std::launch::async, [this] {
        while (!isExitRequested_) {
            prepareFrame();
            update();
            draw();
            endFrame();
        }
    });

    // Loop until there is a quit message from the window or the user.
    while (!isExitRequested_) {
        // Handle the windows messages.
        while (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg_);
            DispatchMessage(&msg_);
        }

        // If windows signals to end the application then exit out.
        if (msg_.message == WM_QUIT) {
            isExitRequested_ = true;
        }

        //auto id = std::this_thread::get_id();
        //std::cout << "Main id: " << id << "\n";
    }

    renderThread.wait();
}

void Game::update() {
}

void Game::updateInternal() {
}

void Game::destroyResources() {
    device_->Release();

    debug_->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
}

void Game::prepareFrame() {
    auto curTime = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime_).count() / 1000000.0f;
    prevTime_ = curTime;

    totalTime_ += deltaTime;
    frameCount_++;

    if (totalTime_ > 1.0f) {
        float fps = frameCount_ / totalTime_;
        totalTime_ = 0.0f;
        SetWindowText(display_.hWnd(), (TEXT("FPS: ") + std::to_wstring(fps)).c_str());
        frameCount_ = 0;
    }

    restoreTargets();

    float color[] = { totalTime_, 0.1f, 0.1f, 1.0f };
    context_->ClearRenderTargetView(renderView_, color);
}

void Game::restoreTargets() {
    context_->OMSetRenderTargets(1, &renderView_, nullptr);
}

void Game::draw() {
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(display_.width());
    viewport.Height = static_cast<float>(display_.height());
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;

    context_->RSSetViewports(1, &viewport);
    context_->OMSetRenderTargets(1, &renderView_, nullptr);

    annotation_->BeginEvent(L"BeginDraw");
    //context_->DrawIndexed(6, 0, 0);
    for (auto&& i : components_) {
        i->draw(context_);
    }
    annotation_->EndEvent();
}

void Game::endFrame() {
    //bool s_EnableVSync = true;
    //UINT PresentInterval = s_EnableVSync ? std::min(4, (int)(s_FrameTime * 60.0f)) : 0;

    swapChain1_->Present(0, DXGI_PRESENT_DO_NOT_WAIT);
}