#pragma once

internal char const RECT_SHADER[] = R"(
// Structures
//

struct Cpu2Vertex {
  float2 pos       : POS;
  float2 scale     : SCALE;
  uint   color     : COL;
  uint   vertex_id : SV_VertexID;
};

struct Vertex2Pixel {
  float4 pos   : SV_POSITION;
  float4 color : COLOR;
};

float4 
unpack_color(uint pack)
{
    float4 color;
    color.r = ((pack >> 24) & 0xFF) / 255.0;
    color.g = ((pack >> 16) & 0xFF) / 255.0;
    color.b = ((pack >> 8)  & 0xFF) / 255.0;
    color.a = ((pack >> 0)  & 0xFF) / 255.0;
    return color;
}

// Vertex Shader
//

Vertex2Pixel 
vs_main(Cpu2Vertex input) {
  // Determine the local vertex position based on the vertexID
  float2 pos;
  uint idx = input.vertex_id % 4;
  if (idx == 0) {
    pos = float2(0.0, 1.0); // Top-left
  } else if (idx == 1) {
    pos = float2(0.0, 0.0); // Bottom-left
  } else if (idx == 2) {
    pos = float2(1.0, 1.0); // Top-right
  } else if (idx == 3) {
    pos = float2(1.0, 0.0); // Bottom-right
  }

  pos *= input.scale;
  pos += input.pos;

  // @ToDo: View, proj matrix here! Rotate about center?
  //

  Vertex2Pixel output;
  output.pos = float4(pos, 0.0, 1.0);
  output.color = float4(1, 0, 0, 1); // unpack_color(input.color);
  return output;
}

// Pixel Shader
//

float4 
ps_main(Vertex2Pixel input) : SV_TARGET {
  return input.color;
};
)";
