#include <dxframework/triangles_game.hpp>
#include <dxframework/triangle_component.hpp>

using namespace dxframework;

triangles_game::triangles_game(renderer& ren, input_device& input_dev)
    : game{ ren, input_dev } {
}

void triangles_game::run_frame_(float delta) {
    renderer_.prepare_frame();
    for (auto& component : components_) {
        component->draw();
    }
    renderer_.end_frame();
}

void triangles_game::run_() {
}

void triangles_game::init_() {
    components_.push_back(std::make_shared<triangle_component>(renderer_,
      buffer<vertex>{ DirectX::XMFLOAT4{ 0.5f, 0.5f, 0.5f, 1.0f },
        DirectX::XMFLOAT4{ -0.5f, -0.5f, 0.5f, 1.0f },
        DirectX::XMFLOAT4{ 0.5f, -0.5f, 0.5f, 1.0f } },
      buffer<color>{ DirectX::XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f },
        DirectX::XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f },
        DirectX::XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f } }));
    components_.back()->initialize();
}

void triangles_game::destroy_() {
}