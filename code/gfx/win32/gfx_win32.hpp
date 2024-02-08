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
  IDXGISwapChain1 *dxgi_swapchain;

  ID3D11Device        *device;
  ID3D11DeviceContext *immediate_context;
  ID3D11DeviceContext *deferred_context;

  ID3D11Texture2D        *framebuffer;
  ID3D11RenderTargetView *framebuffer_rtv;

  ID3D11BlendState *blend_state;

  struct {
    ID3D11Buffer *rect;
  } index_buffer;

  struct {
    ID3D11InputLayout *pos_sz;
  } input_layout;
} global gD3d;
