#pragma once
#include "all_inc.hpp"

// pass initial settings
void
gfx_init(GFX_Opts const &opts) {
  ErrorContext("%ux%u px, vsync=%s"_s8,
               opts.vp_width,
               opts.vp_height,
               opts.vsync ? "on" : "off");
  HRESULT hr;

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
                         &gD3d.context);

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
                           &gD3d.context);

    os_debug_message(
        "Failed to create device on dedicated GPU. Fallback to WARP..\n"_s8);
  }

  ErrorIf(
      FAILED(hr), "Unable to create a device (neither HW nor WARP). hr=0x%X"_s8, hr);

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

  gD3d.dxgi_swapchain->GetBuffer(
      0, __uuidof(ID3D11Texture2D), (void **)&gD3d.framebuffer);
  gD3d.device->CreateRenderTargetView(gD3d.framebuffer, 0, &gD3d.framebuffer_rtv);
}

void
gfx_swap_buffers() {
  ErrorContext("Swapchain stuff..."_s8);
  gD3d.context->ClearState();

  // Render here
  //

  HRESULT hr;
  hr = gD3d.dxgi_swapchain->Present(1, 0);
  ErrorIf(FAILED(hr), "Call to Present failed. hr=0x%X"_s8, hr);
}
