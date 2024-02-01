#pragma once

// Includes and libraries
//
#include <d3d11.h>          // D3D interface
#include <d3dcommon.h>
#include <dxgi.h>           // DirectX driver interface
#include <d3dcompiler.h>    // shader compiler
#include <dxgi.h>
#include <dxgi1_6.h>        // For finding dedicated GPU
#include <dxgiformat.h>
#include <d3d11sdklayers.h> // Debug interface

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// DirectX 11 backend state
//

struct {
  IDXGIFactory6   *dxgi_factory;

  ID3D11Device        *device;  // GPU. Handles objects. Executes commands.
  ID3D11DeviceContext *context; // State manager, generates commands.
  IDXGISwapChain1 *dxgi_swapchain;

  ID3D11Texture2D        *framebuffer;
  ID3D11RenderTargetView *framebuffer_rtv;
} internal gD3d;


