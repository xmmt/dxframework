#include <dxframework/renderer.hpp>

using namespace dxframework;

renderer::renderer(display& disp)
    : initialized_{ false }
    , frame_started_{ false }
    , display_{ disp } {
}

void renderer::init() {
    if (initialized_) {
        throw std::runtime_error("renderer is already initialized");
    }

    create_context_();
    create_back_buffer_();

    initialized_ = true;
}

void renderer::destroy() {
    if (!initialized_) {
        throw std::runtime_error("renderer is not initialized");
    }

    device_->Release();
    debug_->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

    initialized_ = false;
}

void renderer::run_loop(std::function<void(float)> run_frame) const {
    if (!initialized_) {
        throw std::runtime_error("renderer is not initialized");
    }
    display_.run_loop(run_frame);
}

void renderer::prepare_frame() const {
    if (!initialized_) {
        throw std::runtime_error("renderer is not initialized");
    }
    if (frame_started_) {
        throw std::runtime_error("frame is already started");
    }

    auto cur_time = std::chrono::steady_clock::now();
    float delta_time = std::chrono::duration_cast<std::chrono::microseconds>(cur_time - prev_time_).count() / 1000000.0f;
    prev_time_ = cur_time;

    total_time_ += delta_time;
    frame_count_++;

    if (total_time_ > 1.0f) {
        float fps = frame_count_ / total_time_;
        total_time_ = 0.0f;
        auto h_wnd = static_cast<display const&>(display_).h_wnd();
        SetWindowText(h_wnd, std::format(L"FPS: {}", fps).c_str());
        frame_count_ = 0;
    }

    D3D11_VIEWPORT viewport = {
        .TopLeftX = 0.f,
        .TopLeftY = 0.f,
        .Width = static_cast<float>(display_.size().width),
        .Height = static_cast<float>(display_.size().height),
        .MinDepth = 0.f,
        .MaxDepth = 1.f
    };

    context_->RSSetViewports(1, &viewport);
    context_->OMSetRenderTargets(1, &render_view_, depth_stencil_view_.Get());
    //context_->OMSetRenderTargets(1, &render_view_, NULL);

    context_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    CD3D11_RASTERIZER_DESC rast_desc = {};
    rast_desc.CullMode = D3D11_CULL_NONE;
    rast_desc.FillMode = D3D11_FILL_SOLID;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rast_state;
    throw_if_failed(device_->CreateRasterizerState(&rast_desc, rast_state.GetAddressOf()));

    context_->RSSetState(rast_state.Get());

    D3D11_RENDER_TARGET_BLEND_DESC rtbd{
        .BlendEnable = true,
        .SrcBlend = D3D11_BLEND_SRC_ALPHA,
        .DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
        .BlendOp = D3D11_BLEND_OP_ADD,
        .SrcBlendAlpha = D3D11_BLEND_ONE,
        .DestBlendAlpha = D3D11_BLEND_ZERO,
        .BlendOpAlpha = D3D11_BLEND_OP_ADD,
        .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
    };

    D3D11_BLEND_DESC blend_desc{
        .AlphaToCoverageEnable = false,
        .IndependentBlendEnable = false,
        .RenderTarget = { rtbd, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 } },
    };

    Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;
    throw_if_failed(device_->CreateBlendState(&blend_desc, blend_state.GetAddressOf()));

    context_->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);

    CD3D11_DEPTH_STENCIL_DESC depth_stencil_desc(D3D11_DEFAULT);
    depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;
    throw_if_failed(device_->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf()));

    context_->OMSetDepthStencilState(depth_stencil_state.Get(), 0);

    //context_->OMSetRenderTargets(1, &render_view_, depth_stencil_view_.Get());

    color_t color{ total_time_, 0.1f, 0.1f, 1.0f };
    context_->ClearRenderTargetView(render_view_, reinterpret_cast<float*>(&color));
    context_->ClearDepthStencilView(depth_stencil_view_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    frame_started_ = true;
}

void renderer::end_frame() const {
    if (!initialized_) {
        throw std::runtime_error("renderer is not initialized");
    }
    if (!frame_started_) {
        throw std::runtime_error("frame is not started");
    }

    //bool s_EnableVSync = true;
    //UINT PresentInterval = s_EnableVSync ? std::min(4, (int)(s_FrameTime * 60.0f)) : 0;

    auto res = swap_chain1_->Present(1, /* DXGI_PRESENT_DO_NOT_WAIT*/ 0);
    if (res == DXGI_ERROR_WAS_STILL_DRAWING) {
        frame_count_--;
    } else {
        throw_if_failed(res);
    }

    frame_started_ = false;
}

void renderer::draw(
  buffer<vertex>& vertices,
  buffer<color_t>& colors,
  buffer<int>& indices,
  DirectX::XMMATRIX const& view_projection_matrix) const {
    if (!initialized_) {
        throw std::runtime_error("renderer is not initialized");
    }
    if (!frame_started_) {
        throw std::runtime_error("frame is not started");
    }

    index_buffer<int> ib;
    ib.init(device_.Get(), indices.data(), indices.size());

    context_->IASetIndexBuffer(ib.get_buffer(), DXGI_FORMAT_R32_UINT, 0);

    //context_->IASetIndexBuffer(reinterpret_cast<ID3D11Buffer*>(indices.data()), DXGI_FORMAT_R32_UINT, 0);

    //Microsoft::WRL::ComPtr<ID3D11Buffer> pb;
    //Microsoft::WRL::ComPtr<ID3D11Buffer> cb;

    vertex_buffer<DirectX::XMFLOAT4> pb, cb;
    pb.init(device_.Get(), vertices.data(), vertices.size());
    cb.init(device_.Get(), colors.data(), colors.size());

    ID3D11Buffer* v_buffers[] = { pb.get_buffer(), cb.get_buffer() };
    UINT strides[] = { pb.stride(), cb.stride() };
    UINT offsets[] = { 0, 0 };

    context_->IASetVertexBuffers(0, 2, v_buffers, strides, offsets);

    //context_->IASetVertexBuffers(0, 2, vBuffers_, strides_, offsets_);
    //context->RSSetState(rastState_);

    annotation_->BeginEvent(L"BeginDraw");
    context_->DrawIndexed(3, 0, 0);
    annotation_->EndEvent();
}

void renderer::set_shaders(vertex_shader& vs, pixel_shader& ps) {
    context_->VSSetShader(static_cast<ID3D11VertexShader*>(vs.get_shader()), nullptr, 0);
    context_->PSSetShader(static_cast<ID3D11PixelShader*>(ps.get_shader()), nullptr, 0);

    //ID3D11InputLayout* layout;
    //D3D11_INPUT_ELEMENT_DESC input_elements[] = {
    //    D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //    D3D11_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    //};
    //device_->CreateInputLayout(input_elements, 2, vs.get_byte_code()->GetBufferPointer(), vs.get_byte_code()->GetBufferSize(), &layout);
    //context_->IASetInputLayout(layout);

    context_->IASetInputLayout(vs.get_input_layout());
}

void renderer::create_context_() {
    auto display_size = display_.size();
    auto h_wnd = static_cast<display const*>(&display_)->h_wnd();

    DXGI_SWAP_CHAIN_DESC swap_desc = {
        .BufferDesc = {
          .Width = display_size.width,
          .Height = display_size.height,
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
        .OutputWindow = h_wnd,
        .Windowed = true,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
    };

    D3D_FEATURE_LEVEL feature_level[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1 };
    throw_if_failed(D3D11CreateDeviceAndSwapChain(
      nullptr,
      D3D_DRIVER_TYPE_HARDWARE,
      nullptr,
      D3D11_CREATE_DEVICE_DEBUG,
      feature_level,
      1,
      D3D11_SDK_VERSION,
      &swap_desc,
      &swap_chain_,
      &device_,
      nullptr,
      &context_));

    throw_if_failed(swap_chain_->QueryInterface<IDXGISwapChain1>(&swap_chain1_));
    throw_if_failed(context_->QueryInterface<ID3DUserDefinedAnnotation>(&annotation_));
    throw_if_failed(device_->QueryInterface<ID3D11Debug>(&debug_));

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

void renderer::create_back_buffer_() {
    throw_if_failed(swap_chain_->GetBuffer(0, IID_ID3D11Texture2D, (void**)&back_buffer_));
    throw_if_failed(device_->CreateRenderTargetView(back_buffer_, nullptr, &render_view_));

    //describe our Depth/Stencil Buffer
    CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, display_.size().width, display_.size().height);
    depthStencilTextureDesc.MipLevels = 1;
    depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    throw_if_failed(device_->CreateTexture2D(&depthStencilTextureDesc, NULL, depth_stencil_buffer_.GetAddressOf()));
    throw_if_failed(device_->CreateDepthStencilView(depth_stencil_buffer_.Get(), NULL, this->depth_stencil_view_.GetAddressOf()));
    //context_->OMSetRenderTargets(1, &render_view_, NULL /* depth_stencil_view_.Get()*/);
}
