#include "gui.h"
#include <xgraphics.h>
#include <xtl.h>

// Defines
#define PSX_WIDTH 1024
#define PSX_HEIGHT 512

// Externals
extern "C" unsigned char *pPsxScreen;
extern "C" unsigned int g_pPitch;

// Constantes
static const char *vs_entry_point = "main_vertex";
static const char *ps_entry_point = "main_pixel";
static const char *stock_hlsl_program =
    "float4x4 modelViewProj : register(c0);\n"
    "struct VS_DATA {\n"
    "   float4 pos : POSITION;\n"
    "   float2 uv  : TEXCOORD0;\n"
    "};\n"
    "VS_DATA main_vertex(VS_DATA IN) {\n"
    "   VS_DATA OUT;\n"
    "   OUT.pos = mul(IN.pos, modelViewProj);\n"
    "   OUT.uv = IN.uv;\n"
    "   return OUT;\n"
    "}\n"
    "sampler2D tex0 : register(s0);\n"
    "float4 main_pixel(float2 uv : TEXTCOORD0) : COLOR0 {\n"
    "   return tex2D(tex0, uv);\n"
    "}\n";

// D3D9 State
static IDirect3DDevice9 *g_pd3dDevice = nullptr;  // the rendering device
static IDirect3DTexture9 *g_PsxTexture = nullptr; // PSX Texture
static IDirect3DPixelShader9 *pPixelShader = nullptr;
static IDirect3DVertexShader9 *pVertexShader = nullptr;
static IDirect3DVertexDeclaration9 *pVertexDecl = nullptr;
static IDirect3DVertexBuffer9 *pVBuffer;

// Shader State
static const XMMATRIX WorldView = XMMatrixIdentity();
LPD3DXCONSTANTTABLE pPsShaderTable;
LPD3DXCONSTANTTABLE pVsShaderTable;
D3DXHANDLE in_modelViewProj;

// Shader Types
struct shader_input {
  D3DXHANDLE video_size;
  D3DXHANDLE texture_size;
  D3DXHANDLE output_size;
} shader_input;

struct retro_arch_shader_input {
  float video_size[2];
  float texture_size[2];
  float output_size[2];
} retro_arch_shader_input;

struct VERTEX {
  float Position[3];
  float TextureUV[2];
};

static VERTEX Vertices[] = {
    // {  X,	Y,	  Z,	U,	 V	}
    {-1.f, -1.f, 0.0f, 0.0f, 1.0f}, // 1
    {-1.f, 1.f, 0.0f, 0.0f, 0.0f},  // 2
    {1.f, -1.f, 0.0f, 1.0f, 1.0f},  // 3
};
static const DWORD strides = sizeof(VERTEX);

// Define the vertex element format
static const D3DVERTEXELEMENT9 VertexElements[] = {
    {0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,
     0},
    D3DDECL_END()};

// Internal Funtions
static void setupRenderState() {
  g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
  g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
  g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
  g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
  g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
  g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL);
}

static void setupVertexDeclaration() {
  // criar um VertexDeclaration
  HRESULT hr =
      g_pd3dDevice->CreateVertexDeclaration(VertexElements, &pVertexDecl);

  // bindar a VertexDeclaration no VertexShader
  pVertexShader->Bind(0, pVertexDecl, &strides, pPixelShader);
  pVertexDecl->Release();
}

static void setupRenderTexture() {

  // Creating the texture
  g_pd3dDevice->CreateTexture(PSX_WIDTH, PSX_HEIGHT, D3DX_DEFAULT, 0,
                              D3DFMT_LIN_X8R8G8B8, 0, &g_PsxTexture, nullptr);

  // Clear texture to black
  D3DLOCKED_RECT texture_info;
  g_PsxTexture->LockRect(0, &texture_info, nullptr, 0);

  XMemSet128(pPsxScreen, 0, (PSX_WIDTH * PSX_HEIGHT * 4));
  g_PsxTexture->UnlockRect(0);

  g_pPitch = texture_info.Pitch;
}

static bool compileDefaultShaders() {

  // Buffers to hold compiled shaders and error messages
  ID3DXBuffer *pShaderCode = nullptr;
  ID3DXBuffer *pErrorMsg = nullptr;

  // Compiling the vertex shader
  HRESULT hr = D3DXCompileShader(
      stock_hlsl_program, (UINT)strlen(stock_hlsl_program), nullptr, nullptr,
      vs_entry_point, "vs_3_0", 0, &pShaderCode, &pErrorMsg, nullptr);

  if (FAILED(hr)) {
    OutputDebugStringA(pErrorMsg ? (CHAR *)pErrorMsg->GetBufferPointer() : "");
    return false;
  }

  // Create vertex shader.
  g_pd3dDevice->CreateVertexShader((DWORD *)pShaderCode->GetBufferPointer(),
                                   &pVertexShader);

  // Clear the shader code
  pShaderCode->Release();
  pShaderCode = nullptr;

  // Compiling the pixel shader
  hr = D3DXCompileShader(stock_hlsl_program, (UINT)strlen(stock_hlsl_program),
                         nullptr, nullptr, ps_entry_point, "ps_3_0", 0,
                         &pShaderCode, &pErrorMsg, nullptr);
  if (FAILED(hr)) {
    OutputDebugStringA(pErrorMsg ? (CHAR *)pErrorMsg->GetBufferPointer() : "");
    return false;
  }

  // Create pixel shader.
  g_pd3dDevice->CreatePixelShader((DWORD *)pShaderCode->GetBufferPointer(),
                                  &pPixelShader);

  // Clear the shader code
  pShaderCode->Release();
  pShaderCode = nullptr;
}

static void releaseAllShaders() {

  // clear the current shader on d3d state
  g_pd3dDevice->SetVertexShader(nullptr);
  g_pd3dDevice->SetPixelShader(nullptr);

  if (pPixelShader) {
    pPixelShader->Release();
    pPixelShader = nullptr;
  }

  if (pVertexShader) {
    pVertexShader->Release();
    pVertexShader = nullptr;
  }
}

extern void LoadShaderFromFile(const char *filename) {};

// External Implemented Functions
void PcsxSetD3D(IDirect3DDevice9 *device) { g_pd3dDevice = device; }

extern "C" unsigned int VideoInit() {

  // compila o shader padrão pra renderização
  // TODO: fazer isso na build.
  compileDefaultShaders();

  // escreve os parametros do input do shader retroarch
  // tamanho da resolução da entrada (textura do PS1 => sempre 1024x512)
  retro_arch_shader_input.texture_size[0] = PSX_WIDTH;
  retro_arch_shader_input.texture_size[1] = PSX_HEIGHT;

  // tamanho da resolução de saída (tamanho do backbuffer do D3D)
  retro_arch_shader_input.output_size[0] = g_d3dpp.BackBufferWidth;
  retro_arch_shader_input.output_size[1] = g_d3dpp.BackBufferHeight;

  // configurando o vertexbuffer
  g_pd3dDevice->CreateVertexBuffer(sizeof(Vertices), 0, 0, 0, &pVBuffer,
                                   nullptr);

  void *pVerts = nullptr;
  pVBuffer->Lock(0, sizeof(Vertices), &pVerts, 0);
  XMemCpy(pVerts, Vertices, sizeof(Vertices));
  pVBuffer->Unlock();

  // setar os renderstates constantes:
  setupRenderState();

  // setar a vertexDeclaration
  setupVertexDeclaration();

  // criar textura de renderização
  setupRenderTexture();

  return S_OK;
}

void DrawPcsxSurface() {
  const float texture_size[2] = {retro_arch_shader_input.texture_size[0],
                                 retro_arch_shader_input.texture_size[1]};
  const float output_size[2] = {retro_arch_shader_input.output_size[0],
                                retro_arch_shader_input.output_size[1]};
  const float video_size[2] = {retro_arch_shader_input.video_size[0],
                               retro_arch_shader_input.video_size[1]};

  if (in_modelViewProj != NULL)
    pVsShaderTable->SetMatrix(g_pd3dDevice, in_modelViewProj,
                              (D3DXMATRIX *)&WorldView);

  if (shader_input.texture_size != NULL)
    pVsShaderTable->SetFloatArray(g_pd3dDevice, shader_input.texture_size,
                                  texture_size, 2);
  if (shader_input.output_size != NULL)
    pVsShaderTable->SetFloatArray(g_pd3dDevice, shader_input.output_size,
                                  output_size, 2);
  if (shader_input.video_size != NULL)
    pVsShaderTable->SetFloatArray(g_pd3dDevice, shader_input.video_size,
                                  video_size, 2);

  if (shader_input.texture_size != NULL)
    pPsShaderTable->SetFloatArray(g_pd3dDevice, shader_input.texture_size,
                                  texture_size, 2);

  if (shader_input.output_size != NULL)
    pPsShaderTable->SetFloatArray(g_pd3dDevice, shader_input.output_size,
                                  output_size, 2);
  if (shader_input.video_size != NULL)
    pPsShaderTable->SetFloatArray(g_pd3dDevice, shader_input.video_size,
                                  video_size, 2);

  // Sampler State
  const DWORD filter = linearfilter ? D3DTEXF_LINEAR : D3DTEXF_POINT;
  XGSetSamplerAddressStates(g_PsxTexture, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP,
                            D3DTADDRESS_WRAP);
  XGSetSamplerFilterStates(g_PsxTexture, filter, filter, D3DTEXF_NONE, 1);

  // Shaders
  g_pd3dDevice->SetVertexShader(pVertexShader);
  g_pd3dDevice->SetPixelShader(pPixelShader);
  g_pd3dDevice->SetVertexDeclaration(nullptr); // already bound to vertex shader

  // Shader Constants
  g_pd3dDevice->SetVertexShaderConstantF(0, (const float *)&WorldView, 4);

  // Textures - Taking the ownership of registers for a moment
  g_pd3dDevice->GpuOwn(D3DTAG_TEXTUREFETCHCONSTANT(0));
  g_pd3dDevice->GpuSetTextureFetchConstant(0, g_PsxTexture);

  // Draw
  g_pd3dDevice->SetStreamSource(0, pVBuffer, 0, sizeof(VERTEX));
  g_pd3dDevice->DrawPrimitive(D3DPT_QUADLIST, 0, 1);

  // Just after the submission of the drawcall
  g_pd3dDevice->GpuDisown(D3DTAG_TEXTUREFETCHCONSTANT(0));
}

extern "C" void UpdateScrenRes(int x, int y) {
  retro_arch_shader_input.video_size[0] = x;
  retro_arch_shader_input.video_size[1] = y;

  // Update Vertices
  Vertices[0].TextureUV[1] = (float)y / (float)PSX_HEIGHT;

  Vertices[2].TextureUV[0] = (float)x / (float)PSX_WIDTH;
  Vertices[2].TextureUV[1] = (float)y / (float)PSX_HEIGHT;
}

extern "C" void DisplayUpdate() {
  DrawPcsxSurface();
  VideoPresent();
}
