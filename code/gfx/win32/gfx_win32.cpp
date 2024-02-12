#pragma once
#include "all_inc.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
// #define STBI_ASSERT(x) ErrorIf(!(x), "Error in STBI")
#include <stb/stb_image.h>

global void
gfx_resize_image_with_framebuffer(GFX_Image *img) {
  D3d_Image_Node *new_node = arena_alloc<D3d_Image_Node>(gfx_arena);
  new_node->img            = img;
  SLL_insert(gD3d.fb_images, new_node);
}

must_use internal HRESULT
compile_shader(Str8       src,
               Str8       entry_point, // vs_main, ps_main
               Str8       profile,     // vs_5_0, ps_5_0
               ID3DBlob **blob) {
  ErrorContext("entry_point=%S, profile=%S", entry_point, profile);

  read_only local_persist UINT SHADER_FLAGS = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

  ID3DBlob *err_blob = 0;
  HRESULT   hr       = 0;
  hr                 = D3DCompile(src.v,
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

must_use GFX_Image
d3d_load_png(Str8 src) {
  ErrorContext("src=%S", src);

  stbi_set_flip_vertically_on_load(0);
  int x        = 0;
  int y        = 0;
  int ch       = 0;
  // @ToDo: use own functions for file loading
  //
  u8 *img_data = stbi_load((char const *)src.v, &x, &y, &ch, 4);
  Defer { stbi_image_free(img_data); };

  GFX_Image result = gfx_make_image(img_data, (u32)x, (u32)y);
  return result;
}

must_use ID3D11RenderTargetView *
d3d_image_to_rtv(GFX_Image image) {
  ErrorIf(!image.v[0], "Image for render target is not initialized");
  ID3D11Texture2D              *rt_texture = (ID3D11Texture2D *)U64ToPtr(image.v[0]);
  ID3D11RenderTargetView       *rt_view    = 0;
  D3D11_RENDER_TARGET_VIEW_DESC desc       = {
            .Format        = D3D_TEXTURE_FORMAT,
            .ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D,
            .Texture2D     = {.MipSlice = 0},
  };

  HRESULT hr = gD3d.device->CreateRenderTargetView(rt_texture, &desc, &rt_view);
  ErrorIf(FAILED(hr), "Failed to create render target view. %S", os_error_to_user_message(hr));
  return rt_view;
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
    {
        .SemanticName         = "ROT",
        .SemanticIndex        = 0,
        .Format               = DXGI_FORMAT_R32_FLOAT,
        .InputSlot            = 0,
        .AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT,
        .InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA,
        .InstanceDataStepRate = 1,
    },
};

internal D3D11_INPUT_ELEMENT_DESC const INPUT_ELEMENT_LAYOUT_SPRITE[] = {
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
        .SemanticName         = "TEX_RECT",
        .SemanticIndex        = 0,
        .Format               = DXGI_FORMAT_R32G32B32A32_FLOAT,
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
    {
        .SemanticName         = "ROT",
        .SemanticIndex        = 0,
        .Format               = DXGI_FORMAT_R32_FLOAT,
        .InputSlot            = 0,
        .AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT,
        .InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA,
        .InstanceDataStepRate = 1,
    },
};

// pass initial settings
global void
gfx_init(GFX_Opts const &opts) {
  ErrorContext("%ux%u px, vsync=%s", opts.vp_width, opts.vp_height, opts.vsync ? "on" : "off");

  gfx_arena = make_arena(true);

  HRESULT hr = 0;

  hr = CreateDXGIFactory1(__uuidof(IDXGIFactory6), (void **)&gD3d.dxgi_factory);
  ErrorIf(FAILED(hr), "CreateDXGIFactory1 failed. %S", os_error_to_user_message(hr));

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

  ErrorIf(FAILED(hr),
          "Unable to create a device (neither HW nor WARP). %S",
          os_error_to_user_message(hr));

// Enable break-on-error.
//
#if !defined(NDEBUG)
  ID3D11InfoQueue *info = 0;
  hr                    = gD3d.device->QueryInterface(__uuidof(ID3D11InfoQueue), (void **)&info);
  if (SUCCEEDED(hr)) {
    info->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
    info->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
  } else {
    os_debug_message("Failed to enable dbug interface.\n"_s8);
  }
  info->Release();
#endif

  // Create swap chain.
  //
  DXGI_SWAP_CHAIN_DESC1 const sc_desc = {
      .Width       = opts.vp_width,
      .Height      = opts.vp_height,
      .Format      = D3D_TEXTURE_FORMAT,
      .SampleDesc  = {.Count = 1, .Quality = 0},
      .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
      .BufferCount = 2,
      .SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD,
  };
  hr = gD3d.dxgi_factory->CreateSwapChainForHwnd(
      gD3d.device, w32_hwnd, &sc_desc, 0, 0, &gD3d.dxgi_swapchain);
  ErrorIf(FAILED(hr), "Unable to create the swapchain. %S", os_error_to_user_message(hr));

  // Acquire the framebuffer.
  //

  hr = gD3d.dxgi_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&gD3d.framebuffer);
  ErrorIf(FAILED(hr), "Failed to acquire the framebuffer. %S", os_error_to_user_message(hr));

  // Create deferred context.
  //
  hr = gD3d.device->CreateDeferredContext(0, &gD3d.deferred_context);
  ErrorIf(FAILED(hr), "Failed to create deferred context. %S", os_error_to_user_message(hr));

  // Create blend state.
  //
  D3D11_BLEND_DESC bs_desc = {};
  // Set up for rendering onto a transparent texture
  // Src: https://stackoverflow.com/a/27932112/6696102
  //
  bs_desc.RenderTarget[0] = {
      .BlendEnable           = 1,
      .SrcBlend              = D3D11_BLEND_SRC_ALPHA,
      .DestBlend             = D3D11_BLEND_INV_SRC_ALPHA,
      .BlendOp               = D3D11_BLEND_OP_ADD,
      .SrcBlendAlpha         = D3D11_BLEND_INV_DEST_ALPHA,
      .DestBlendAlpha        = D3D11_BLEND_ONE,
      .BlendOpAlpha          = D3D11_BLEND_OP_ADD,
      .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
  };
  hr = gD3d.device->CreateBlendState(&bs_desc, &gD3d.blend_state);
  ErrorIf(FAILED(hr), "Unable to create blend state. %S", os_error_to_user_message(hr));

  // Create rasterizer state.
  //

  D3D11_RASTERIZER_DESC const rasterizer_desc = {
      .FillMode = D3D11_FILL_SOLID,
      .CullMode = D3D11_CULL_NONE,
  };

  hr = gD3d.device->CreateRasterizerState(&rasterizer_desc, &(gD3d.rasterizer_state));
  ErrorIf(FAILED(hr), "Unable to create rasterizer state. %S", os_error_to_user_message(hr));

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
  ErrorIf(FAILED(hr), "Unable to create rect index buffer. %S", os_error_to_user_message(hr));

  // Load and compile shaders.
  //

  ID3DBlob *vs_blob = 0;
  ID3DBlob *ps_blob = 0;

  // Compile shaders.
  //
  {
    ErrorContext("Compiling GFX_RECT_SHADER");
    hr = compile_shader(str8_cstr(GFX_RECT_SHADER), "vs_main"_s8, "vs_5_0"_s8, &vs_blob);

    ErrorIf(FAILED(hr),
            "Failed to compile vertex shader. %s",
            (char const *)vs_blob->GetBufferPointer());

    hr = compile_shader(str8_cstr(GFX_RECT_SHADER), "ps_main"_s8, "ps_5_0"_s8, &ps_blob);

    ErrorIf(FAILED(hr),
            "Failed to compile pixel shader. %s",
            (char const *)ps_blob->GetBufferPointer());

    hr = gD3d.device->CreateVertexShader(
        vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), 0, &(gD3d.rect.vs));
    ErrorIf(FAILED(hr), "Failed to create vertex shader. %S", os_error_to_user_message(hr));
    Defer { vs_blob->Release(); };

    hr = gD3d.device->CreatePixelShader(
        ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), 0, &(gD3d.rect.ps));
    ErrorIf(FAILED(hr), "Failed to create pixel shader. %S", os_error_to_user_message(hr));
    Defer { ps_blob->Release(); };

    // Create input layout of the Vertex Shader.
    //
    hr = gD3d.device->CreateInputLayout(INPUT_ELEMENT_LAYOUT_RECT,
                                        ArrayCount(INPUT_ELEMENT_LAYOUT_RECT),
                                        vs_blob->GetBufferPointer(),
                                        vs_blob->GetBufferSize(),
                                        &(gD3d.rect.input_layout));
    ErrorIf(FAILED(hr), "Failed to create input layout. %S", os_error_to_user_message(hr));
  }

  {
    ErrorContext("Compiling GFX_SPRITE_SHADER");
    hr = compile_shader(str8_cstr(GFX_SPRITE_SHADER), "vs_main"_s8, "vs_5_0"_s8, &vs_blob);

    ErrorIf(FAILED(hr),
            "Failed to compile vertex shader. %s",
            (char const *)vs_blob->GetBufferPointer());

    hr = compile_shader(str8_cstr(GFX_SPRITE_SHADER), "ps_main"_s8, "ps_5_0"_s8, &ps_blob);

    ErrorIf(FAILED(hr),
            "Failed to compile pixel shader. %s",
            (char const *)ps_blob->GetBufferPointer());

    hr = gD3d.device->CreateVertexShader(
        vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), 0, &(gD3d.sprite.vs));
    ErrorIf(FAILED(hr), "Failed to create vertex shader. %S", os_error_to_user_message(hr));
    Defer { vs_blob->Release(); };

    hr = gD3d.device->CreatePixelShader(
        ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), 0, &(gD3d.sprite.ps));
    ErrorIf(FAILED(hr), "Failed to create pixel shader. %S", os_error_to_user_message(hr));
    Defer { ps_blob->Release(); };

    // Create input layout of the Vertex Shader.
    //
    hr = gD3d.device->CreateInputLayout(INPUT_ELEMENT_LAYOUT_SPRITE,
                                        ArrayCount(INPUT_ELEMENT_LAYOUT_SPRITE),
                                        vs_blob->GetBufferPointer(),
                                        vs_blob->GetBufferSize(),
                                        &(gD3d.sprite.input_layout));
    ErrorIf(FAILED(hr), "Failed to create input layout. %S", os_error_to_user_message(hr));
  }

  {
    ErrorContext("Compiling GFX_COMBINE_SHADER");
    hr = compile_shader(str8_cstr(GFX_COMBINE_SHADER), "vs_main"_s8, "vs_5_0"_s8, &vs_blob);

    ErrorIf(FAILED(hr),
            "Failed to compile vertex shader. %s",
            (char const *)vs_blob->GetBufferPointer());

    hr = compile_shader(str8_cstr(GFX_COMBINE_SHADER), "ps_main"_s8, "ps_5_0"_s8, &ps_blob);

    ErrorIf(FAILED(hr),
            "Failed to compile pixel shader. %s",
            (char const *)ps_blob->GetBufferPointer());

    hr = gD3d.device->CreateVertexShader(
        vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), 0, &(gD3d.combine.vs));
    ErrorIf(FAILED(hr), "Failed to create vertex shader. %S", os_error_to_user_message(hr));
    Defer { vs_blob->Release(); };

    hr = gD3d.device->CreatePixelShader(
        ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), 0, &(gD3d.combine.ps));
    ErrorIf(FAILED(hr), "Failed to create pixel shader. %S", os_error_to_user_message(hr));
    Defer { ps_blob->Release(); };
  }

  {
    ErrorContext("Compiling GFX_BLUR_SHADER");
    hr = compile_shader(str8_cstr(GFX_BLUR_SHADER), "vs_main"_s8, "vs_5_0"_s8, &vs_blob);

    ErrorIf(FAILED(hr),
            "Failed to compile vertex shader. %s",
            (char const *)vs_blob->GetBufferPointer());

    hr = compile_shader(str8_cstr(GFX_BLUR_SHADER), "ps_main"_s8, "ps_5_0"_s8, &ps_blob);

    ErrorIf(FAILED(hr),
            "Failed to compile pixel shader. %s",
            (char const *)ps_blob->GetBufferPointer());

    hr = gD3d.device->CreateVertexShader(
        vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), 0, &(gD3d.post_fx.blur.vs));
    ErrorIf(FAILED(hr), "Failed to create vertex shader. %S", os_error_to_user_message(hr));
    Defer { vs_blob->Release(); };

    hr = gD3d.device->CreatePixelShader(
        ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), 0, &(gD3d.post_fx.blur.ps));
    ErrorIf(FAILED(hr), "Failed to create pixel shader. %S", os_error_to_user_message(hr));
    Defer { ps_blob->Release(); };
  }

  {
    ErrorContext("Compiling GFX_VIGNETTE_SHADER");
    hr = compile_shader(str8_cstr(GFX_VIGNETTE_SHADER), "vs_main"_s8, "vs_5_0"_s8, &vs_blob);

    ErrorIf(FAILED(hr),
            "Failed to compile vertex shader. %s",
            (char const *)vs_blob->GetBufferPointer());

    hr = compile_shader(str8_cstr(GFX_VIGNETTE_SHADER), "ps_main"_s8, "ps_5_0"_s8, &ps_blob);

    ErrorIf(FAILED(hr),
            "Failed to compile pixel shader. %s",
            (char const *)ps_blob->GetBufferPointer());

    hr = gD3d.device->CreateVertexShader(
        vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), 0, &(gD3d.post_fx.vignette.vs));
    ErrorIf(FAILED(hr), "Failed to create vertex shader. %S", os_error_to_user_message(hr));
    Defer { vs_blob->Release(); };

    hr = gD3d.device->CreatePixelShader(
        ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), 0, &(gD3d.post_fx.vignette.ps));
    ErrorIf(FAILED(hr), "Failed to create pixel shader. %S", os_error_to_user_message(hr));
    Defer { ps_blob->Release(); };
  }

  // Create samplers
  //

  {
    ErrorContext("Linear sampler");
    D3D11_SAMPLER_DESC const desc = {
        //.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR, // Linear filtering
        .Filter         = D3D11_FILTER_MIN_MAG_MIP_POINT,
        .AddressU       = D3D11_TEXTURE_ADDRESS_WRAP,
        .AddressV       = D3D11_TEXTURE_ADDRESS_WRAP,
        .AddressW       = D3D11_TEXTURE_ADDRESS_WRAP,
        .ComparisonFunc = D3D11_COMPARISON_NEVER,
        .MinLOD         = 0,
        .MaxLOD         = D3D11_FLOAT32_MAX,
    };

    hr = gD3d.device->CreateSamplerState(&desc, &(gD3d.linear_sampler));
    ErrorIf(FAILED(hr), "%S", os_error_to_user_message(hr));
  }

  {
    ErrorContext("Create common constants buffer");

    // Create common constants
    //
    gD3d.common_constants.data.projection =
        ortho_proj((f32)os_gfx_surface_width(), (f32)os_gfx_surface_height());

    // Upload common constants to GPU
    //
    D3D11_BUFFER_DESC desc = {
        .ByteWidth      = sizeof(D3d_Common_Constants),
        .Usage          = D3D11_USAGE_DYNAMIC,
        .BindFlags      = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    };

    D3D11_SUBRESOURCE_DATA data = {
        .pSysMem = &gD3d.common_constants.data,
    };

    hr = gD3d.device->CreateBuffer(&desc, &data, &(gD3d.common_constants.buffer));
    ErrorIf(FAILED(hr), "CreateBuffer failed. %S", os_error_to_user_message(hr));
  }

  {
    ErrorContext("Create post fx constants buffer");

    // Create common constants
    //
    gD3d.post_fx_constants.data = {
        .resolution =
            {
                .width  = (f32)os_gfx_surface_width(),
                .height = (f32)os_gfx_surface_height(),
            },
        .time    = os_seconds_since_startup(),
        .quality = 1,
    };

    // Upload common constants to GPU
    //
    D3D11_BUFFER_DESC desc = {
        .ByteWidth      = sizeof(D3d_Post_Fx_Constants),
        .Usage          = D3D11_USAGE_DYNAMIC,
        .BindFlags      = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    };

    D3D11_SUBRESOURCE_DATA data = {
        .pSysMem = &gD3d.post_fx_constants.data,
    };

    hr = gD3d.device->CreateBuffer(&desc, &data, &(gD3d.post_fx_constants.buffer));
    ErrorIf(FAILED(hr), "CreateBuffer failed. %S", os_error_to_user_message(hr));
  }
}

global void
gfx_resize(u32 new_width, u32 new_height) {
  ErrorContext("new_width=%u, new_height=%u", new_width, new_height);

  new_width  = Clamp(1, new_width, 16384);
  new_height = Clamp(1, new_height, 16384);

  HRESULT hr = 0;
  // Release all references to back buffers.
  //
  {
    ErrorContext("Clear state");
    gD3d.framebuffer->Release();

    ID3D11CommandList *command_list = 0;
    hr = gD3d.deferred_context->FinishCommandList(FALSE, &command_list);
    ErrorIf(FAILED(hr), "Failed to finish command list. %S", os_error_to_user_message(hr));
    command_list->Release();
    gD3d.immediate_context->ClearState();
    gD3d.deferred_context->ClearState();
  }

  // Resize the swap chain buffers.
  //
  hr = gD3d.dxgi_swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
  ErrorIf(FAILED(hr) && FAILED(gD3d.device->GetDeviceRemovedReason()),
          "ResizeBuffers failed. %S",
          os_error_to_user_message(hr));

  // Reacquire the framebuffer.
  //
  hr = gD3d.dxgi_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&(gD3d.framebuffer));
  ErrorIf(FAILED(hr), "Failed to reacquire the framebuffer. %S", os_error_to_user_message(hr));

  // Update projection matrix
  //
  gD3d.common_constants.data.projection = ortho_proj((f32)new_width, (f32)new_height);

  // Update common constants
  //
  D3D11_MAPPED_SUBRESOURCE mapped_consts;
  gD3d.deferred_context->Map(
      gD3d.common_constants.buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_consts);
  MemoryCopy(mapped_consts.pData, &gD3d.common_constants.data, sizeof(D3d_Common_Constants));
  gD3d.deferred_context->Unmap(gD3d.common_constants.buffer, 0);

  // Update textures that are the size of framebuffer
  //
  for (D3d_Image_Node *it = gD3d.fb_images; it; it = it->next) {
    ID3D11Texture2D *old_tex = (ID3D11Texture2D *)U64ToPtr(it->img->v[0]);

    D3D11_TEXTURE2D_DESC desc;
    old_tex->GetDesc(&desc);

    // We need this for copying the old texture into new one.
    //
    u32 const old_width  = desc.Width;
    u32 const old_height = desc.Height;

    desc.Width  = new_width;
    desc.Height = new_height;

    // Create the new texture
    ID3D11Texture2D *new_tex = 0; // The new texture
    hr                       = gD3d.device->CreateTexture2D(&desc, 0, &new_tex);
    ErrorIf(FAILED(hr), "Failed to recreate some texture!");

    // Copy the old texture into the new one (do we need it?)
    //
    D3D11_BOX region = {
        .right  = Min(new_width, old_width),
        .bottom = Min(new_height, old_height),
        .back   = 1,
    };
    gD3d.deferred_context->CopySubresourceRegion(new_tex, 0, 0, 0, 0, old_tex, 0, &region);

    old_tex->Release();
    it->img->v[0] = PtrToU64(new_tex);
  }

  gD3d.dxgi_swapchain->Present(1, 0);
}

global void
gfx_swap_buffers() {
  HRESULT hr = 0;

  // Clear the state.
  //
  gD3d.immediate_context->ClearState();

  // Execute rendering commands
  //
  {
    ErrorContext("Execute rendering commands");
    gD3d.deferred_context->ClearState();
    ID3D11CommandList *command_list = 0;
    hr = gD3d.deferred_context->FinishCommandList(FALSE, &command_list);
    ErrorIf(FAILED(hr), "Failed to finish command list. %S", os_error_to_user_message(hr));
    gD3d.immediate_context->ExecuteCommandList(command_list, FALSE);
    command_list->Release();
  }

  // Swap buffers
  //
  {
    ErrorContext("Swap Buffers");
    hr = gD3d.dxgi_swapchain->Present(1, 0);
    ErrorIf(FAILED(hr), "Call to Present failed. %S", os_error_to_user_message(hr));
  }
}

must_use global GFX_Image
gfx_make_empty_image() {
  return {};
}

must_use global GFX_Image
gfx_make_image(u8 *data, u32 width, u32 height) {
  ErrorContext("data=%s, width=%d, height=%d", data ? "yes" : "nope", (int)width, (int)height);

  D3D11_TEXTURE2D_DESC desc = {
      .Width      = width,
      .Height     = height,
      .MipLevels  = 1,
      .ArraySize  = 1,
      .Format     = D3D_TEXTURE_FORMAT,
      .SampleDesc = {.Count = 1},
      .Usage      = D3D11_USAGE_DEFAULT,
      .BindFlags  = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
  };

  D3D11_SUBRESOURCE_DATA initial_data = {
      .pSysMem     = data,
      .SysMemPitch = width * 4, // 4 channels
  };

  ID3D11Texture2D *tex = 0;

  HRESULT hr = 0;
  hr         = gD3d.device->CreateTexture2D(&desc, data ? (&initial_data) : 0, &tex);
  ErrorIf(FAILED(hr), "Failed to create render target texture. %S", os_error_to_user_message(hr));

  GFX_Image img;
  img.v[0] = PtrToU64(tex);
  return img;
}

global void
gfx_release_image(GFX_Image img) {
  ID3D11Texture2D *tex = (ID3D11Texture2D *)U64ToPtr(img.v[0]);
  tex->Release();
}

must_use global fvec2
gfx_image_size(GFX_Image img) {
  ID3D11Texture2D     *tex = (ID3D11Texture2D *)U64ToPtr(img.v[0]);
  D3D11_TEXTURE2D_DESC desc;
  tex->GetDesc(&desc);
  return {.width = (f32)desc.Width, .height = (f32)desc.Height};
}

must_use global GFX_Batch *
gfx_make_batch(GFX_Material_Type material) {
  ErrorContext("material=%d", (int)material);

  GFX_Batch *batch = arena_alloc<GFX_Batch>(gfx_arena);
  batch->type      = material;
  batch->objects.v   = arena_alloc_array<GFX_Object>(gfx_arena, 64);
  batch->objects.cap = 64;

  UINT byte_width = (UINT)batch->objects.cap;
  switch (material) {
    default: {
      // @ToDo: implement other materials
      InvalidPath;
    } break;

    case GFX_MaterialType_Rect: {
      byte_width *= sizeof(GFX_Rect_Instance);
    } break;

    case GFX_MaterialType_Sprite: {
      byte_width *= sizeof(GFX_Sprite_Instance);
    } break;
  }

  ID3D11Buffer     *instances = 0;
  D3D11_BUFFER_DESC desc      = {
           .ByteWidth      = byte_width,
           .Usage          = D3D11_USAGE_DYNAMIC,
           .BindFlags      = D3D11_BIND_VERTEX_BUFFER,
           .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
  };
  HRESULT hr = gD3d.device->CreateBuffer(&desc, 0, &instances);
  ErrorIf(FAILED(hr), "Failed to create instance buffer. %S", os_error_to_user_message(hr));
  batch->instances.v[0] = PtrToU64(instances);
  return batch;
}

global void
gfx_release_batch(GFX_Batch *batch) {
  // Not implemented
  Unused(batch);
  InvalidPath;
}

global void
gfx_batch_push(GFX_Batch *batch, GFX_Object object) {
  if (!batch) {
    return;
  }

  ErrorIf(batch->type != object.material.type,
          "Materials don't match. Batch material is %d, object material is %d.",
          (int)batch->type,
          (int)object.material.type);

  // @Note: scale the underlaying instance buffer too!
  ErrorIf(batch->objects.sz == batch->objects.cap, "We did not implement growing batches yet!");
  batch->objects.v[batch->objects.sz] = object;
  batch->objects.sz++;
}

global void
gfx_batch_draw(GFX_Batch *batch, GFX_Image target) {
  Assert(batch);
  Assert(target.v[0]);
  ErrorContext("material=%d, sz=%d", (int)batch->type, (int)batch->objects.sz);

  // Create render target view.
  //
  ID3D11RenderTargetView *rt_view = d3d_image_to_rtv(target);
  Defer { rt_view->Release(); };

  // Copy the instance data to the instance buffer of appropiate layout.
  //

  Assert(batch->type == GFX_MaterialType_Rect || batch->type == GFX_MaterialType_Sprite);

  Scratch_Buffer scratch = scratch_begin(gContext.frame_arena);
  Defer { scratch_end(&scratch); };

  if (batch->type == GFX_MaterialType_Rect) {
    // @ToDo: respect layering! Sort by layer somehow.
    //
    GFX_Rect_Instance *instances =
        arena_alloc_array<GFX_Rect_Instance>(gContext.frame_arena, batch->objects.sz);
    for (u64 i = 0; i < batch->objects.sz; i++) {
      GFX_Object const &object = batch->objects.v[i];

      instances[i] = {
          .pos   = object.pos,
          .scale = object.sz,
          .col   = object.material.rect.color.v,
          .rot   = object.rot,
      };
    }

    // Map the memory to GPU
    //

    D3D11_MAPPED_SUBRESOURCE mapped_instances;
    ID3D11Buffer            *instances_buffer = (ID3D11Buffer *)U64ToPtr(batch->instances.v[0]);
    gD3d.deferred_context->Map(instances_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_instances);
    MemoryCopy(mapped_instances.pData, instances, sizeof(GFX_Rect_Instance) * batch->objects.sz);
    gD3d.deferred_context->Unmap(instances_buffer, 0);

    // Draw the batch.
    //

    UINT stride = sizeof(GFX_Rect_Instance);
    UINT offset = 0;

    gD3d.deferred_context->ClearState();
    gD3d.deferred_context->IASetVertexBuffers(0, 1, &instances_buffer, &stride, &offset);
    gD3d.deferred_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gD3d.deferred_context->IASetIndexBuffer(gD3d.index_buffer.rect, DXGI_FORMAT_R32_UINT, 0);
    gD3d.deferred_context->IASetInputLayout(gD3d.rect.input_layout);

    gD3d.deferred_context->VSSetShader(gD3d.rect.vs, 0, 0);
    gD3d.deferred_context->VSSetConstantBuffers(0, 1, &(gD3d.common_constants.buffer));

    gD3d.deferred_context->PSSetShader(gD3d.rect.ps, 0, 0);

    // @ToDo: Handle zoom and offsets!
    //
    fvec2 const    rt_size = gfx_image_size(target);
    D3D11_VIEWPORT vp      = {
             .Width  = rt_size.width,
             .Height = rt_size.height,
    };

    gD3d.deferred_context->RSSetViewports(1, &vp);
    gD3d.deferred_context->RSSetState(gD3d.rasterizer_state);

    gD3d.deferred_context->OMSetBlendState(gD3d.blend_state, 0, 0xffffffff);
    gD3d.deferred_context->OMSetRenderTargets(1, &rt_view, 0);

    gD3d.deferred_context->DrawIndexedInstanced(6, (UINT)batch->objects.sz, 0, 0, 0);
  } else if (batch->type == GFX_MaterialType_Sprite) {
    // @ToDo: respect layering! Sort by layer somehow.
    //
    GFX_Sprite_Instance *instances =
        arena_alloc_array<GFX_Sprite_Instance>(gContext.frame_arena, batch->objects.sz);
    for (u64 i = 0; i < batch->objects.sz; i++) {
      GFX_Object const &object = batch->objects.v[i];

      instances[i] = {
          .pos   = object.pos,
          .scale = object.sz,
          .tex_coords =
              {
                  .x = (f32)object.material.sprite.tex_rect.x,
                  .y = (f32)object.material.sprite.tex_rect.y,
                  .z = (f32)object.material.sprite.tex_rect.w,
                  .w = (f32)object.material.sprite.tex_rect.h,
              },
          .color = object.material.sprite.color.v,
          .rot   = object.rot,
      };
    }

    // Map the memory to GPU
    //

    D3D11_MAPPED_SUBRESOURCE mapped_instances;
    ID3D11Buffer            *instances_buffer = (ID3D11Buffer *)U64ToPtr(batch->instances.v[0]);
    gD3d.deferred_context->Map(instances_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_instances);
    MemoryCopy(mapped_instances.pData, instances, sizeof(GFX_Sprite_Instance) * batch->objects.sz);
    gD3d.deferred_context->Unmap(instances_buffer, 0);

    // Create the SRV for the source image
    //
    ErrorIf(!batch->data.sprite.texture.v[0], "Source image not set!");
    ID3D11Texture2D          *tex = (ID3D11Texture2D *)U64ToPtr(batch->data.sprite.texture.v[0]);
    ID3D11ShaderResourceView *srv = 0;

    D3D11_SHADER_RESOURCE_VIEW_DESC const srv_desc = {
        .Format        = D3D_TEXTURE_FORMAT,
        .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
        .Texture2D     = {.MipLevels = 1},
    };

    HRESULT hr = 0;
    hr = gD3d.device->CreateShaderResourceView(tex, &srv_desc, &srv);
    Defer{srv->Release();};
    ErrorIf(FAILED(hr), "Failed to create SRV for A. %S", os_error_to_user_message(hr));

    // Draw the batch.
    //

    UINT stride = sizeof(GFX_Sprite_Instance);
    UINT offset = 0;

    gD3d.deferred_context->ClearState();
    gD3d.deferred_context->IASetVertexBuffers(0, 1, &instances_buffer, &stride, &offset);
    gD3d.deferred_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gD3d.deferred_context->IASetIndexBuffer(gD3d.index_buffer.rect, DXGI_FORMAT_R32_UINT, 0);
    gD3d.deferred_context->IASetInputLayout(gD3d.sprite.input_layout);

    gD3d.deferred_context->VSSetShaderResources(0, 1, &srv);
    gD3d.deferred_context->VSSetShader(gD3d.sprite.vs, 0, 0);
    gD3d.deferred_context->VSSetConstantBuffers(0, 1, &(gD3d.common_constants.buffer));

    gD3d.deferred_context->PSSetShaderResources(0, 1, &srv);
    gD3d.deferred_context->PSSetShader(gD3d.sprite.ps, 0, 0);
    gD3d.deferred_context->PSSetSamplers(0, 1, &(gD3d.linear_sampler));

    // @ToDo: Handle zoom and offsets!
    //
    fvec2 const    rt_size = gfx_image_size(target);
    D3D11_VIEWPORT vp      = {
             .Width  = rt_size.width,
             .Height = rt_size.height,
    };

    gD3d.deferred_context->RSSetViewports(1, &vp);
    gD3d.deferred_context->RSSetState(gD3d.rasterizer_state);

    gD3d.deferred_context->OMSetBlendState(gD3d.blend_state, 0, 0xffffffff);
    gD3d.deferred_context->OMSetRenderTargets(1, &rt_view, 0);

    gD3d.deferred_context->DrawIndexedInstanced(6, (UINT)batch->objects.sz, 0, 0, 0);
  }
}

must_use internal GFX_Image
gfx_rg_execute_operations(GFX_RG_Operation *operations, u32 count) {
  ErrorContext("count=%d", (int)count);

  for (u32 i = 0; i < count; i++) {
    GFX_RG_Operation &op = operations[i];

    ErrorContext("i=%d, op.type=%d", (int)i, (int)op.type);
    switch (op.type) {
      default: {
        InvalidPath;
      } break;

      case GFX_RG_OpType_ClearRenderTargets: {
        ErrorContext("ClearRenderTargets, num_targets=%d", (int)op.input.clear.num_targets);
        for (u32 j = 0; j < op.input.clear.num_targets; j++) {
          ErrorContext("j=%d", (int)j);
          // Get the render target.
          //
          GFX_Image *target = op.input.clear.targets[j];
          ErrorIf(!target, "target is a NULL");
          ID3D11RenderTargetView *rt_view = d3d_image_to_rtv(*target);
          Defer { rt_view->Release(); };

          local_persist const fvec4 clear_color = {.r = 0.0, .g = 0.0, .b = 0.0, .a = 0.0};
          gD3d.deferred_context->ClearRenderTargetView(rt_view, clear_color.v);
        }
      } break;

      case GFX_RG_OpType_Batch: {
        gfx_batch_draw(op.input.batch.batch, *op.out);
      } break;

      case GFX_RG_OpType_PostFx: {
        gfx_apply_post_fx(op.input.post_fx.fx, *op.input.post_fx.src, *op.out);
      } break;

      case GFX_RG_OpType_CombineImages: {
        gfx_combine_images(*op.input.combine_images.a, *op.input.combine_images.b, *op.out);
      } break;
    }
  }

  // Assuming that last operation result is the final image to render.
  //
  GFX_Image graph_result = *operations[count - 1].out;
  return graph_result;
}

global void
gfx_combine_images(GFX_Image a, GFX_Image b, GFX_Image target) {
  ErrorContext("Combining stuff...");
  ErrorIf(!a.v[0], "Image A not set!");
  ErrorIf(!b.v[0], "Image B not set!");
  ErrorIf(!target.v[0], "Target not set!");

  // Create render target view.
  //
  ID3D11RenderTargetView *rt_view = d3d_image_to_rtv(target);
  Defer { rt_view->Release(); };

  // Create the SRVs for images a and b.
  //
  ID3D11Texture2D          *tex_a = (ID3D11Texture2D *)U64ToPtr(a.v[0]);
  ID3D11Texture2D          *tex_b = (ID3D11Texture2D *)U64ToPtr(b.v[0]);
  ID3D11ShaderResourceView *srv_a = 0;
  ID3D11ShaderResourceView *srv_b = 0;

  D3D11_SHADER_RESOURCE_VIEW_DESC const srv_desc = {
      .Format        = D3D_TEXTURE_FORMAT,
      .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
      .Texture2D     = {.MipLevels = 1},
  };

  HRESULT hr = 0;
  hr = gD3d.device->CreateShaderResourceView(tex_a, &srv_desc, &srv_a);
  ErrorIf(FAILED(hr), "Failed to create SRV for A. %S", os_error_to_user_message(hr));
  hr = gD3d.device->CreateShaderResourceView(tex_b, &srv_desc, &srv_b);
  ErrorIf(FAILED(hr), "Failed to create SRV for B. %S", os_error_to_user_message(hr));

  Defer { srv_a->Release(); };
  Defer { srv_b->Release(); };

  gD3d.deferred_context->ClearState();
  gD3d.deferred_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  gD3d.deferred_context->IASetIndexBuffer(gD3d.index_buffer.rect, DXGI_FORMAT_R32_UINT, 0);
  gD3d.deferred_context->IASetInputLayout(0);

  gD3d.deferred_context->VSSetShader(gD3d.combine.vs, 0, 0);
  gD3d.deferred_context->PSSetShader(gD3d.combine.ps, 0, 0);

  gD3d.deferred_context->PSSetShaderResources(0, 1, &srv_a);
  gD3d.deferred_context->PSSetShaderResources(1, 1, &srv_b);

  gD3d.deferred_context->PSSetSamplers(0, 1, &(gD3d.linear_sampler));

  // @Robustness: what should be the viewport here?
  fvec2 const    rt_size = gfx_image_size(target);
  D3D11_VIEWPORT vp      = {
           .Width  = rt_size.width,
           .Height = rt_size.height,
  };

  gD3d.deferred_context->RSSetViewports(1, &vp);
  gD3d.deferred_context->RSSetState(gD3d.rasterizer_state);

  // gD3d.deferred_context->OMSetBlendState(gD3d.blend_state, 0, 0xffffffff);
  gD3d.deferred_context->OMSetRenderTargets(1, &rt_view, 0);

  gD3d.deferred_context->DrawIndexed(6, 0, 0);
}

void
gfx_apply_post_fx(GFX_Fx fx, GFX_Image src, GFX_Image target) {
  ErrorContext("fx.type = %d", (int)fx.type);
  ErrorIf(!src.v[0], "Src not set!");
  ErrorIf(!target.v[0], "Target not set!");

  // Create render target view.
  //
  ID3D11RenderTargetView *rt_view = d3d_image_to_rtv(target);
  Defer { rt_view->Release(); };

  // Create the SRVs for images a and b.
  //
  ID3D11Texture2D          *tex = (ID3D11Texture2D *)U64ToPtr(src.v[0]);
  ID3D11ShaderResourceView *srv = 0;

  D3D11_SHADER_RESOURCE_VIEW_DESC const srv_desc = {
      .Format        = D3D_TEXTURE_FORMAT,
      .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
      .Texture2D     = {.MipLevels = 1},
  };

  HRESULT hr = 0;
  hr = gD3d.device->CreateShaderResourceView(tex, &srv_desc, &srv);
  ErrorIf(FAILED(hr), "Failed to create SRV. %S", os_error_to_user_message(hr));
  Defer { srv->Release(); };

  // Update Post FX constants
  //
  gD3d.post_fx_constants.data = {
      .resolution =
          {
              .width  = (f32)os_gfx_surface_width(), // @ToDo: this should be the src texture size?
              .height = (f32)os_gfx_surface_height(),
          },
      .time    = os_seconds_since_startup(),
      .quality = fx.strength,
  };
  D3D11_MAPPED_SUBRESOURCE mapped_consts;
  gD3d.deferred_context->Map(
      gD3d.post_fx_constants.buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_consts);
  MemoryCopy(mapped_consts.pData, &gD3d.post_fx_constants.data, sizeof(D3d_Post_Fx_Constants));
  gD3d.deferred_context->Unmap(gD3d.post_fx_constants.buffer, 0);

  gD3d.deferred_context->ClearState();
  gD3d.deferred_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  gD3d.deferred_context->IASetIndexBuffer(gD3d.index_buffer.rect, DXGI_FORMAT_R32_UINT, 0);
  gD3d.deferred_context->IASetInputLayout(0);

  // Bind correct shaders
  //

  switch (fx.type) {
    default: {
      InvalidPath;
    } break;
    case GFX_PostFXType_Blur: {
      gD3d.deferred_context->VSSetShader(gD3d.post_fx.blur.vs, 0, 0);
      gD3d.deferred_context->PSSetShader(gD3d.post_fx.blur.ps, 0, 0);
    } break;

    case GFX_PostFXType_Vignette: {
      gD3d.deferred_context->VSSetShader(gD3d.post_fx.vignette.vs, 0, 0);
      gD3d.deferred_context->PSSetShader(gD3d.post_fx.vignette.ps, 0, 0);
    } break;
  }

  gD3d.deferred_context->PSSetConstantBuffers(0, 1, &(gD3d.post_fx_constants.buffer));
  gD3d.deferred_context->PSSetShaderResources(0, 1, &srv);

  gD3d.deferred_context->PSSetSamplers(0, 1, &(gD3d.linear_sampler));

  // @Robustness: what should be the viewport here?
  fvec2 const    rt_size = gfx_image_size(target);
  D3D11_VIEWPORT vp      = {
           .Width  = rt_size.width,
           .Height = rt_size.height,
  };

  gD3d.deferred_context->RSSetViewports(1, &vp);
  gD3d.deferred_context->RSSetState(gD3d.rasterizer_state);

  gD3d.deferred_context->OMSetRenderTargets(1, &rt_view, 0);

  gD3d.deferred_context->DrawIndexed(6, 0, 0);
}
