#include "pch.h"

#include "Game.hpp"
#include "TriangleComponent.hpp"

using namespace DirectX;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

int WINAPI WinMain(
  _In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPSTR lpCmdline,
  _In_ int nShowCmd) {

#if _DEBUG
    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
#endif // _DEBUG

    std::vector<std::shared_ptr<GameComponent>> components;
    components.emplace_back(new TriangleComponent{
      "MiniTri.fx",
      {},
      { { "TEST", "1" }, { "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)" } },
      {
        XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
        XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),
        XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),
      },
      {
        XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
        XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
      } });
    components.emplace_back(new TriangleComponent{
      "MiniTri.fx",
      {},
      { { "TEST", "1" }, { "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)" } },
      {
        XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
        XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),
        XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),
      },
      {
        XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
        XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
      } });
    //components.emplace_back(new TriangleComponent{
    //  "MiniTri.fx",
    //  {},
    //  {},
    //  {
    //    XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),
    //    XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),
    //    XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),
    //  },
    //  {
    //    XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
    //    XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
    //    XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //  } });
    //components.emplace_back(new TriangleComponent{
    //  "MiniTri.fx",
    //  {},
    //  {},
    //  {
    //    XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
    //    XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),
    //    XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),
    //  },
    //  {
    //    XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
    //    XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
    //    XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //  } });

    DisplayWin32 display{ hInstance, L"Game" };
    InputDevice inputDevice{};
    Game game{ display, inputDevice, std::move(components) };
    game.run();
    game.destroyResources();

    return 0;
}
