#include <dxframework/model.hpp>
#include <dxframework/game_component.hpp>
#include <dxframework/transform.hpp>

namespace dxframework {

class guitar_component : public game_component {
public:
    guitar_component(renderer& renderer, std::string file_path);
    ~guitar_component() = default;

public:
    void destroy_resources() override;
    void draw(DirectX::XMMATRIX const& view_projection_matrix) override;
    void initialize() override;
    void reload() override;
    void update() override;

public:
    transform& get_transform();

private:
    model model_;
    transform transform_;
    std::string file_path_;
    static pixel_shader ps_;
    static vertex_shader vs_;
    static constant_buffer<CB_VS_vertexshader> cb_;
};

} // namespace dxframework