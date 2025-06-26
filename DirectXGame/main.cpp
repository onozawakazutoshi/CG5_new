#include "Shader.h"
#include <KamataEngine.h>
#include <Windows.h>
#include <d3dcompiler.h>
#include "RootSignature.h"
#include "PipelineState.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

using namespace KamataEngine;


void SetupPipelineState(PipelineState& pipelineState,RootSignature& rs,Shader& vs,Shader& ps);

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
};
VertexData vertices[] = {
    {{-1.0f, 1.0f, 0.0f, 1.0f}, {0,0}},
    {{3.0f, 1.0f, 0.0f, 1.0f}, {2,0}},
    {{-1.0f, -3.0f, 0.0f, 1.0f}, {0,2}}
    };
uint16_t indices[] = {0, 1, 2};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	KamataEngine::Initialize();
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

	RootSignature rs;
	rs.Crete();

	Shader vs;

	vs.LoadDxc(L"Resources/shaders/TestVS.hlsl", L"vs_6_0");

	assert(vs.GetDxcBlob() != nullptr);

	Shader ps;

	ps.LoadDxc(L"Resources/shaders/TestSP.hlsl", L"ps_6_0");

	assert(ps.GetDxcBlob() != nullptr);

	// ID3DBlob* errorBlob = nullptr;

	PipelineState pipelinestate;
	SetupPipelineState(pipelinestate,rs,vs,ps);

	VertexBuffer vb;

	vb.Create(sizeof(vertices), sizeof(vertices[0]));

	IndexBuffer ib;
	ib.Create(sizeof(indices), sizeof(indices[0]));
	
	uint16_t* pGpuindex = nullptr;
	ib.Get()->Map(0, nullptr, reinterpret_cast<void**>(&pGpuindex));
	for (int i = 0; i < _countof(vertices); ++i) {
		pGpuindex[i] = indices[i];
	}
	
	VertexData* pGpuVertices = nullptr;
	vb.Get()->Map(0, nullptr, reinterpret_cast<void**>(&pGpuVertices));
	for (int i = 0; i < _countof(vertices); ++i) {
		pGpuVertices[i] = vertices[i];
	}

	D3D12_VIEWPORT viewport{};

	viewport.Width = (float)dxCommon->GetBackBufferWidth();
	viewport.Height = (float)dxCommon->GetBackBufferHeight();
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	D3D12_RECT scissorRect{};

	scissorRect.left = 0;
	scissorRect.right = dxCommon->GetBackBufferWidth();
	scissorRect.top = 0;
	scissorRect.bottom = dxCommon->GetBackBufferHeight();

	MSG msg{};

	while (true) {
		if (KamataEngine::Update()) {

			break;
		}

		dxCommon->PreDraw();
		commandList->SetGraphicsRootSignature(rs.Get());
		commandList->SetPipelineState(pipelinestate.Get());
		commandList->IASetVertexBuffers(0, 1, vb.GetView());
		commandList->IASetIndexBuffer(ib.GetView());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//commandList->DrawInstanced(3, 1, 0, 0);
		commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);
		dxCommon->PostDraw();
	}
	
	KamataEngine::Finalize();
	return 0;
}

void SetupPipelineState(PipelineState& pipelineState, RootSignature& rs, Shader& vs, Shader& ps) 
{
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[2] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = _countof(inputElementDesc);


	D3D12_BLEND_DESC blendDesc{};

	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rs.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = {vs.GetDxcBlob()->GetBufferPointer(), vs.GetDxcBlob()->GetBufferSize()};
	graphicsPipelineStateDesc.PS = {ps.GetDxcBlob()->GetBufferPointer(), ps.GetDxcBlob()->GetBufferSize()};
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	pipelineState.Create(graphicsPipelineStateDesc);

}
