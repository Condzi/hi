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

// Common constants used in the backend
//
global read_only DXGI_FORMAT D3D_TEXTURE_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

// DirectX 11 backend state
//

struct D3d_Common_Constants {
  // Updated on window resize.
  //
  fmat4 projection;
};

struct D3d_Post_Fx_Constants {
  fvec2 resolution;
  f32   time    = 0;
  f32   quality = 0;
};

struct D3d_Image_Node {
  D3d_Image_Node *next;
  GFX_Image      *img;
};

struct D3d {
  IDXGIFactory6   *dxgi_factory;
  IDXGISwapChain1 *dxgi_swapchain;

  ID3D11Device        *device;
  ID3D11DeviceContext *immediate_context;
  ID3D11DeviceContext *deferred_context;

  ID3D11Texture2D        *framebuffer;
  ID3D11RenderTargetView *framebuffer_rtv;

  // @ToDo: different blend states!
  //
  ID3D11BlendState      *blend_state;
  ID3D11RasterizerState *rasterizer_state;

  // @ToDo: different sampler types!
  //
  ID3D11SamplerState *linear_sampler;

  struct {
    ID3D11Buffer *rect;
  } index_buffer;

  struct {
    ID3D11VertexShader *vs;
    ID3D11PixelShader  *ps;
    ID3D11InputLayout  *input_layout;
  } rect;

  struct {
    ID3D11VertexShader *vs;
    ID3D11PixelShader  *ps;
    ID3D11InputLayout  *input_layout;
  } sprite;

  struct {
    ID3D11VertexShader *vs;
    ID3D11PixelShader  *ps;
  } combine;

  struct {
    D3d_Common_Constants data;
    ID3D11Buffer        *buffer;
  } common_constants;

  struct {
    D3d_Post_Fx_Constants data;
    ID3D11Buffer         *buffer;
  } post_fx_constants;

  // @ToDo: all PostFX have the same vertex shader!
  //
  struct {
    struct {
      ID3D11VertexShader *vs;
      ID3D11PixelShader  *ps;
    } blur;

    struct {
      ID3D11VertexShader *vs;
      ID3D11PixelShader  *ps;
    } vignette;
  } post_fx;

  D3d_Image_Node *fb_images;
} global gD3d;

struct GFX_Rect_Instance {
  fvec2 pos;
  fvec2 scale;
  u32   col = 0;
  f32   rot = 0;
};

struct GFX_Sprite_Instance {
  fvec2 pos;
  fvec2 scale;
  fvec4 tex_coords; // Lazy way since our tex coords are 16 bit xD
  u32   color = 0;
  f32   rot   = 0;
};

// Helper functions.
//

// On failure `blob` is set to error messages. Otherwise, it's the shader.
//
must_use internal HRESULT
compile_shader(Str8       src,
               Str8       entry_point, // vs_main, ps_main
               Str8       profile,     // vs_5_0, ps_5_0
               ID3DBlob **blob);

must_use GFX_Image
d3d_load_png(Str8 src);

must_use ID3D11RenderTargetView*
d3d_image_to_rtv(GFX_Image image);
