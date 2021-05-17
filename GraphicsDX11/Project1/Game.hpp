#pragma once

#include "pch.h"

#include "DisplayWin32.hpp"
#include "GameComponent.hpp"
#include "InputDevice.hpp"

class Game {
public:
    Game(DisplayWin32& display, InputDevice& inputDevice, std::vector<std::shared_ptr<GameComponent>> components);

public:
    void createBackBuffer();
    void destroyResources();
    void draw();
    void endFrame();
    void exit();
    void initialize();
    static LRESULT CALLBACK messageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
    void prepareFrame();
    void prepareResources();
    void restoreTargets();
    void run();
    void update();
    void updateInternal();

private:
    std::wstring name_{ L"Game" };

    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    ID3D11DeviceContext* context_;

    ID3D11Debug* debug_;
    ID3DUserDefinedAnnotation* annotation_;
    //int debugAnnotation_;

    DisplayWin32& display_;
    InputDevice& inputDevice_;
    HINSTANCE instance_;

    //int renderSRV_;
    ID3D11RenderTargetView* renderView_;
    ID3D11Texture2D* backBuffer_;
    IDXGISwapChain* swapChain_;
    IDXGISwapChain1* swapChain1_;

    int screenResized_;
    std::atomic_bool isExitRequested_{ false };
    MSG msg_{};

    std::vector<std::shared_ptr<GameComponent>> components_;

    std::chrono::time_point<std::chrono::steady_clock> prevTime_;
    int startTime_;
    float totalTime_{ 0 };
    unsigned int frameCount_{ 0 };

    //ID3D11RenderTargetView* rtv;
};