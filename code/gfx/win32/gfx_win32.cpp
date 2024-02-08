#pragma once
#include "all_inc.hpp"

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

void
gfx_swap_buffers() {
  ErrorContext("Swapchain stuff..."_s8);
  HRESULT hr = 0;

  // Clear the state.
  //
  gD3d.immediate_context->ClearState();
  gD3d.deferred_context->ClearState();

  fvec4 clear_color = {0.6f, 0.25f, 0.29f, 1.0f};
  gD3d.deferred_context->ClearRenderTargetView(gD3d.framebuffer_rtv, clear_color.v);

  // Render here
  //

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
