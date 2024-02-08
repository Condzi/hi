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
  ID3D11RasterizerState *rasterizer_state;

  struct {
    ID3D11Buffer *rect;
  } index_buffer;

  struct {
    ID3D11VertexShader* vs;
    ID3D11PixelShader* ps;
    ID3D11InputLayout *input_layout;
 
  } rect; 
  struct {
  } input_layout;
} global gD3d;

struct Rect_Instance {
  fvec2 pos;
  fvec2 scale;
  u32   col = 0;
};

global Arena *gfx_arena;

// Helper functions.
//

// On failure `blob` is set to error messages. Otherwise, it's the shader.
//
must_use internal HRESULT
compile_shader(Str8       src,
               Str8       entry_point, // vs_main, ps_main
               Str8       profile,     // vs_5_0, ps_5_0
               ID3DBlob **blob);
