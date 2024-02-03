#pragma once

// Includes and libraries
//

#include <dxgi.h>
#include <dxgi1_6.h>
#include <dxgiformat.h>

#pragma comment(lib, "dxgi.lib")

#include <d3d11.h>
#include <d3d11sdklayers.h>

#include <d3dcommon.h>

#include <d3dcompiler.h>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// DirectX 11 backend state
//

struct D3d {
  IDXGIFactory6 *dxgi_factory;

  ID3D11Device        *device;  // GPU. Handles objects. Executes commands.
  ID3D11DeviceContext *context; // Immediate context.
  IDXGISwapChain1     *dxgi_swapchain;

  ID3D11Texture2D        *framebuffer;
  ID3D11RenderTargetView *framebuffer_rtv;
} global gD3d;
