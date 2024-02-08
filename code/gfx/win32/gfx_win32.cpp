#pragma once
#include "all_inc.hpp"

must_use internal HRESULT
compile_shader(Str8       src,
               Str8       entry_point, // vs_main, ps_main
               Str8       profile,     // vs_5_0, ps_5_0
               ID3DBlob **blob) {
  ErrorContext("entry_point = %s, profile = %s"_s8, entry_point.v, profile.v);

  read_only local_persist UINT SHADER_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

  ID3DBlob *err_blob = 0;
  HRESULT   hr       = 0;
  hr                 = ::D3DCompile(src.v,
                    src.sz,
                    0,
                    0,
                    D3D_COMPILE_STANDARD_FILE_INCLUDE,
                    (LPCSTR)entry_point.v,
                    (LPCSTR)profile.v,
                    SHADER_FLAGS,
                    0,
                    blob,
                    &err_blob);

  // On failure, the error buffer to the output.
  //
  if (FAILED(hr)) {
    (*blob) = err_blob;
  }

  return hr;
}

// Input Element Tables
//

internal D3D11_INPUT_ELEMENT_DESC const INPUT_ELEMENT_LAYOUT_RECT[] = {
    {
        .SemanticName         = "POS",
        .SemanticIndex        = 0,
        .Format               = DXGI_FORMAT_R32G32_FLOAT,
        .InputSlot            = 0,
        .AlignedByteOffset    = 0,
        .InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA,
        .InstanceDataStepRate = 1,
    },
    {
        .SemanticName         = "SCALE",
        .SemanticIndex        = 0,
        .Format               = DXGI_FORMAT_R32G32_FLOAT,
        .InputSlot            = 0,
        .AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT,
        .InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA,
        .InstanceDataStepRate = 1,
    },
    {
        .SemanticName         = "COL",
        .SemanticIndex        = 0,
        .Format               = DXGI_FORMAT_R32_UINT,
        .InputSlot            = 0,
        .AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT,
        .InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA,
        .InstanceDataStepRate = 1,
    },
};

// pass initial settings
void
gfx_init(GFX_Opts const &opts) {
  ErrorContext("%ux%u px, vsync=%s"_s8,
               opts.vp_width,
               opts.vp_height,
               opts.vsync ? "on" : "off");
  HRESULT hr = 0;

  hr = CreateDXGIFactory1(__uuidof(IDXGIFactory6), (void **)&gD3d.dxgi_factory);
  ErrorIf(FAILED(hr), "CreateDXGIFactory1 returned 0x%X"_s8, hr);

  // Try to find dedicated GPU.
  //
  IDXGIAdapter1 *adapter = 0;
  for (UINT i = 0;; i++) {
    hr = gD3d.dxgi_factory->EnumAdapterByGpuPreference(
        i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));

    if (hr == DXGI_ERROR_NOT_FOUND || adapter) {
      break;
    }
  }

  // Device creation parameters.
  //

  D3D_DRIVER_TYPE driver_type = D3D_DRIVER_TYPE_UNKNOWN;
  if (!adapter) {
    driver_type = D3D_DRIVER_TYPE_WARP;
    os_debug_message("GPU not found -- fallback to WARP.\n"_s8);
  }

  UINT creation_flags = 0;
#if !defined(NDEBUG)
  creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  D3D_FEATURE_LEVEL feature_levels[] = {D3D_FEATURE_LEVEL_11_0};

  // Create DirectX device.
  //
  hr = D3D11CreateDevice(adapter,
                         driver_type,
                         0,
                         creation_flags,
                         feature_levels,
                         ArrayCount(feature_levels),
                         D3D11_SDK_VERSION,
                         &gD3d.device,
                         0,
                         &gD3d.immediate_context);

  // Try WARP driver if HW is not available.
  // @ToDo: Warn about this!
  //
  if (FAILED(hr) && driver_type == D3D_DRIVER_TYPE_UNKNOWN) {
    hr = D3D11CreateDevice(adapter,
                           D3D_DRIVER_TYPE_WARP,
                           0,
                           creation_flags,
                           feature_levels,
                           ArrayCount(feature_levels),
                           D3D11_SDK_VERSION,
                           &gD3d.device,
                           0,
                           &gD3d.immediate_context);

    os_debug_message("Failed to create device on dedicated GPU. Fallback to WARP..\n"_s8);
  }

  ErrorIf(FAILED(hr), "Unable to create a device (neither HW nor WARP). hr=0x%X"_s8, hr);

// Enable break-on-error.
//
#if !defined(NDEBUG)
  ID3D11InfoQueue *info = 0;
  hr = gD3d.device->QueryInterface(__uuidof(ID3D11InfoQueue), (void **)&info);
  if (SUCCEEDED(hr)) {
    info->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
    info->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
  } else {
    os_debug_message("Failed to enable dbug interface.\n"_s8);
  }
#endif

  // Create swap chain.
  //
  DXGI_SWAP_CHAIN_DESC1 const sc_desc = {
      .Width       = opts.vp_width,
      .Height      = opts.vp_height,
      .Format      = DXGI_FORMAT_B8G8R8A8_UNORM,
      .SampleDesc  = {.Count = 1, .Quality = 0},
      .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
      .BufferCount = 2,
      .SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD,
  };
  hr = gD3d.dxgi_factory->CreateSwapChainForHwnd(
      gD3d.device, w32_hwnd, &sc_desc, 0, 0, &gD3d.dxgi_swapchain);
  ErrorIf(FAILED(hr), "Unable to create the swapchain. hr=0x%X"_s8, hr);

  // Acquire the framebuffer.
  //

  hr = gD3d.dxgi_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&gD3d.framebuffer);
  ErrorIf(FAILED(hr), "Failed to acquire the framebuffer. hr=0x%X."_s8, hr);
  hr = gD3d.device->CreateRenderTargetView(gD3d.framebuffer, 0, &gD3d.framebuffer_rtv);
  ErrorIf(FAILED(hr), "Failed to create rtv for framebuffer. hr=0x%X."_s8, hr);

  // Create deferred context.
  //
  hr = gD3d.device->CreateDeferredContext(0, &gD3d.deferred_context);
  ErrorIf(FAILED(hr), "Failed to create deferred context. hr=0x%X"_s8, hr);

  // Create blend state.
  //
  D3D11_BLEND_DESC bs_desc = {};
  bs_desc.RenderTarget[0]  = {
       .BlendEnable           = 1,
       .SrcBlend              = D3D11_BLEND_SRC_ALPHA,
       .DestBlend             = D3D11_BLEND_INV_SRC_ALPHA,
       .BlendOp               = D3D11_BLEND_OP_ADD,
       .SrcBlendAlpha         = D3D11_BLEND_ONE,
       .DestBlendAlpha        = D3D11_BLEND_ZERO,
       .BlendOpAlpha          = D3D11_BLEND_OP_ADD,
       .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
  };
  hr = gD3d.device->CreateBlendState(&bs_desc, &gD3d.blend_state);
  ErrorIf(FAILED(hr), "Unable to create blend state. hr=0x%X."_s8, hr);

  // Create rasterizer state.
  //

  D3D11_RASTERIZER_DESC const rasterizer_desc = {
      .FillMode = D3D11_FILL_SOLID,
      .CullMode = D3D11_CULL_NONE,
  };

  hr = gD3d.device->CreateRasterizerState(&rasterizer_desc, &(gD3d.rasterizer_state));
  ErrorIf(FAILED(hr), "Unable to create rasterizer state. hr=0x%X"_s8, hr);

  // Create index buffer
  //

  // top-left, bottom-left, top-right; top-right, bottom-left, bottom-right
  //
  u32 const         indices[] = {0, 1, 2, 2, 1, 3};
  D3D11_BUFFER_DESC ib_desc   = {
        .ByteWidth = sizeof(indices),
        .Usage     = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_INDEX_BUFFER,
  };
  D3D11_SUBRESOURCE_DATA ib_data = {
      .pSysMem = indices,
  };
  hr = gD3d.device->CreateBuffer(&ib_desc, &ib_data, &gD3d.index_buffer.rect);  
  ErrorIf(FAILED(hr), "Unable to create rect index buffer. hr=0x%X."_s8, hr);

  // Load and compile shaders.
  //

  ID3DBlob *vs_blob = 0;
  ID3DBlob *ps_blob = 0;

  // Compile shaders.
  //

  hr = compile_shader(str8_cstr(RECT_SHADER), "vs_main"_s8, "vs_5_0"_s8, &vs_blob);

  ErrorIf(FAILED(hr),
          "Failed to compile vertex shader: %s"_s8,
          (char const *)vs_blob->GetBufferPointer());

  hr = compile_shader(str8_cstr(RECT_SHADER), "ps_main"_s8, "ps_5_0"_s8, &ps_blob);

  ErrorIf(FAILED(hr),
          "Failed to compile pixel shader: %s"_s8,
          (char const *)ps_blob->GetBufferPointer());

  // Create shaders. No circle shader yet!
  //

  hr = gD3d.device->CreateVertexShader(
      vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), 0, &(gD3d.rect.vs));
  ErrorIf(FAILED(hr), "Failed to create vertex shader. hr=0x%X"_s8, hr);
  Defer { vs_blob->Release(); };

  hr = gD3d.device->CreatePixelShader(
      ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), 0, &(gD3d.rect.ps));
  ErrorIf(FAILED(hr), "Failed to create pixel shader. hr=0x%X"_s8, hr);
  Defer { ps_blob->Release(); };

  // Create input layout of the Vertex Shader.
  //
  hr = gD3d.device->CreateInputLayout(INPUT_ELEMENT_LAYOUT_RECT,
                                      ArrayCount(INPUT_ELEMENT_LAYOUT_RECT),
                                      vs_blob->GetBufferPointer(),
                                      vs_blob->GetBufferSize(),
                                      &(gD3d.rect.input_layout));

  ErrorIf(FAILED(hr), "Failed to create input layout. hr=0x%X"_s8, hr);
} 

void
gfx_resize(u32 new_width, u32 new_height) {
  ErrorContext("new_width=%u, new_height=%u"_s8, new_width, new_height);

  // Release all references to back buffers.
  //
  gD3d.framebuffer_rtv->Release();
  gD3d.framebuffer->Release();

  HRESULT hr = 0;

  // Resize the swap chain buffers.
  //
  hr = gD3d.dxgi_swapchain->ResizeBuffers(0, new_width, new_height, DXGI_FORMAT_UNKNOWN, 0);
  ErrorIf(FAILED(hr), "ResizeBuffers failed. hr=0x%X."_s8, hr);

  // Reacquire the framebuffer.
  //
  hr = gD3d.dxgi_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&gD3d.framebuffer);
  ErrorIf(FAILED(hr), "Failed to reacquire the framebuffer. hr=0x%X."_s8, hr);
  hr = gD3d.device->CreateRenderTargetView(gD3d.framebuffer, 0, &gD3d.framebuffer_rtv);
  ErrorIf(FAILED(hr), "Failed to recreate rtv for framebuffer. hr=0x%X."_s8, hr);
}

struct Rect_Instance {
  fvec2 pos;
  fvec2 scale;
  u32   col = 0;
};

void
gfx_swap_buffers() {
  ErrorContext("Swapchain stuff..."_s8);
  HRESULT hr = 0;

  local_persist Rect_Instance objects[] = {
      {
          .pos   = {.x = 0, .y = 0},
          .scale = {.x = 0.25f, .y = 0.25f},
          .col   = 0xFF0000FF,
      },
      {
          .pos   = {.x = 0.1f, .y = 0.1f},
          .scale = {.x = 0.25f, .y = 0.25f},
          .col   = 0x00FF0080,
      },
  };
  local_persist ID3D11Buffer *instance_buffer = 0;
  if (!instance_buffer) {
    D3D11_BUFFER_DESC desc = {
        .ByteWidth = sizeof(objects),
        .Usage     = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
    };
    D3D11_SUBRESOURCE_DATA data = {.pSysMem = objects};
    hr                          = gD3d.device->CreateBuffer(&desc, &data, &instance_buffer);
    ErrorIf(FAILED(hr), "Failed to create instance buffer. hr=0x%X."_s8, hr);
  }

  // Clear the state.
  //
  gD3d.immediate_context->ClearState();
  gD3d.deferred_context->ClearState();

  fvec4 clear_color = {0.6f, 0.25f, 0.29f, 1.0f};
  gD3d.deferred_context->ClearRenderTargetView(gD3d.framebuffer_rtv, clear_color.v);

  // Render here
  //

  UINT stride = sizeof(Rect_Instance);
  UINT offset = 0;

  gD3d.deferred_context->OMSetBlendState(gD3d.blend_state, 0, 0xffffffff);
  gD3d.deferred_context->IASetVertexBuffers(0, 1, &instance_buffer, &stride, &offset);
  gD3d.deferred_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  gD3d.deferred_context->IASetIndexBuffer(gD3d.index_buffer.rect, DXGI_FORMAT_R32_UINT, 0);
  gD3d.deferred_context->IASetInputLayout(gD3d.rect.input_layout);

  gD3d.deferred_context->VSSetShader(gD3d.rect.vs, 0, 0);
  gD3d.deferred_context->PSSetShader(gD3d.rect.ps, 0, 0);

  D3D11_VIEWPORT vp = {
      .TopLeftX = 0,
                       .TopLeftY = 0,
                       .Width    = (f32)os_gfx_surface_width(),
                       .Height   = (f32)os_gfx_surface_height(),
  };

  gD3d.deferred_context->RSSetViewports(1, &vp);
  gD3d.deferred_context->RSSetState(gD3d.rasterizer_state);

  gD3d.deferred_context->OMSetRenderTargets(1, &gD3d.framebuffer_rtv, 0);

  gD3d.deferred_context->DrawIndexedInstanced(6, ArrayCount(objects), 0, 0, 0);

  // Execute rendering commands
  //
  ID3D11CommandList *command_list = 0;
  hr                              = gD3d.deferred_context->FinishCommandList(FALSE, &command_list);
  ErrorIf(FAILED(hr), "Failed to finish command list. hr=0x%X."_s8, hr);
  gD3d.immediate_context->ExecuteCommandList(command_list, FALSE);

  // Swap buffers
  //
  hr = gD3d.dxgi_swapchain->Present(1, 0);
  ErrorIf(FAILED(hr), "Call to Present failed. hr=0x%X"_s8, hr);
}
