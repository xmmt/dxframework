#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include <RendererDirectX.hpp>
#include <DisplayWin32.hpp>

using namespace GraphicsFramework;

RendererDirectX::RendererDirectX(Display const& display)
    : Renderer{ display } {
}

RenderResult<void> RendererDirectX::init_() {
    {
        auto res = createContext_();
        if (res.isErr()) {
            return res;
        }
    }
    {
        auto res = createBackBuffer_();
        if (res.isErr()) {
            return res;
        }
    }
    return Ok();
}

RenderResult<void> RendererDirectX::destroy_() {
    device_->Release();
    debug_->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    return Ok();
}

RenderResult<void> RendererDirectX::createContext_() {
    auto displaySizeRes = display_.size();
    if (displaySizeRes.isErr()) {
        return Err(RendererError::fromDisplayError(displaySizeRes.unwrapErr()));
    }
    auto&& displaySize = displaySizeRes.unwrap();
    auto hWndRes = static_cast<DisplayWin32 const*>(&display_)->hWnd();
    if (hWndRes.isErr()) {
        return Err(RendererError::fromDisplayError(hWndRes.unwrapErr()));
    }
    auto&& hWnd = hWndRes.unwrap();

    DXGI_SWAP_CHAIN_DESC swapDesc = {
        .BufferDesc = {
          .Width = displaySize.width,
          .Height = displaySize.height,
          .RefreshRate = {
            .Numerator = 60,
            .Denominator = 1,
          },
          .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
          .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
          .Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
        },
        .SampleDesc = {
          .Count = 1,
          .Quality = 0,
        },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 2,
        .OutputWindow = hWnd,
        .Windowed = true,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
    };

    D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1 };
    auto res = D3D11CreateDeviceAndSwapChain(
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
      &context_);
    if (FAILED(res)) {
        return Err(RendererError{
          .kind = RendererError::Kind::INIT_FAILED,
          .text = std::format("can not create device and swapchain, result: {}", res) });
    }

    res = swapChain_->QueryInterface<IDXGISwapChain1>(&swapChain1_);
    if (FAILED(res)) {
        return Err(RendererError{
          .kind = RendererError::Kind::INIT_FAILED,
          .text = std::format("can not query swapchain1 from swapchain, result: {}", res) });
    }
    res = context_->QueryInterface<ID3DUserDefinedAnnotation>(&annotation_);
    if (FAILED(res)) {
        return Err(RendererError{
          .kind = RendererError::Kind::INIT_FAILED,
          .text = std::format("can not query annotation from context, result: {}", res) });
    }
    res = device_->QueryInterface<ID3D11Debug>(&debug_);
    if (FAILED(res)) {
        return Err(RendererError{
          .kind = RendererError::Kind::INIT_FAILED,
          .text = std::format("can not query debug from device, result: {}", res) });
    }

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
    return Ok();
}

RenderResult<void> RendererDirectX::createBackBuffer_() {
    auto res = swapChain_->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBuffer_);
    if (FAILED(res)) {
        return Err(RendererError{
          .kind = RendererError::Kind::INIT_FAILED,
          .text = std::format("can not get buffer from swapchain, result: {}", res) });
    }
    res = device_->CreateRenderTargetView(backBuffer_, nullptr, &renderView_);
    if (FAILED(res)) {
        return Err(RendererError{
          .kind = RendererError::Kind::INIT_FAILED,
          .text = std::format("can not create RenderTargetView from device, result: {}", res) });
    }
    return Ok();
}

RenderResult<void> RendererDirectX::prepareFrame_() const {
    auto curTime = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime_).count() / 1000000.0f;
    prevTime_ = curTime;

    totalTime_ += deltaTime;
    frameCount_++;

    if (totalTime_ > 1.0f) {
        float fps = frameCount_ / totalTime_;
        totalTime_ = 0.0f;
        auto hWndRes = static_cast<DisplayWin32 const&>(display_).hWnd();
        if (hWndRes.isOk()) {
            SetWindowText(hWndRes.unwrap(), std::format(L"FPS: {}", fps).c_str());
        }
        frameCount_ = 0;
    }

    restoreTargets_();

    Color color{ totalTime_, 0.1f, 0.1f, 1.0f };
    context_->ClearRenderTargetView(renderView_, reinterpret_cast<float*>(&color));

    return Ok();
}

RenderResult<void> RendererDirectX::endFrame_() const {
    //bool s_EnableVSync = true;
    //UINT PresentInterval = s_EnableVSync ? std::min(4, (int)(s_FrameTime * 60.0f)) : 0;

    auto res = swapChain1_->Present(1, /* DXGI_PRESENT_DO_NOT_WAIT*/ 0);
    if (res == DXGI_ERROR_WAS_STILL_DRAWING) {
        frameCount_--;
    } else if (FAILED(res)) {
        return Err(RendererError{
          .kind = RendererError::Kind::DRAW_ERROR,
          .text = std::format("swapchain presnt failed, result: {}", res) });
    }

    return Ok();
}

RenderResult<void> RendererDirectX::restoreTargets_() const {
    context_->OMSetRenderTargets(1, &renderView_, nullptr);
    return Ok();
}

RenderResult<void> RendererDirectX::draw_(
  Buffer<Vertex> const& vertices,
  Buffer<Color> const& colors,
  Buffer<int> const& indices,
  Material& material) const {

    return Ok();
}