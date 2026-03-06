#include "CRenderer.hpp"
#include "systems/logging/CLogSystem.hpp"
#include "engine/graphicscontext/CGraphicsContext.hpp"
#include "utils/defines.hpp"
#include "utils/BufferPerObject_t.hpp"

CRenderer& CRenderer::GetInstance() {
	static CRenderer Instance;
	return Instance;
}

bool CRenderer::Initialize() {
	if (!LoadShadersFromFile()) {
		LOG_ERROR("Failed to load shaders.");
		return false;
	}

	if (!CreateWorldViewProjectionBuffer()) {
		LOG_ERROR("Failed to create World View Projection buffer.");
		return false;
	}

	LOG_INFO("Renderer successfully initialized.");
	return true;
}

void CRenderer::UpdateSceneComponents(float flDeltaTime) {
	m_PlayerCamera.Update(flDeltaTime);
}

void CRenderer::RenderScene() {

}

void CRenderer::RenderModel(CModelResourceData* pModelResource, DirectX::XMMATRIX mtWorldMatrix) {

}

void CRenderer::AddToStaticBuffers(CModelResourceData* pResourceData) {
	if (pResourceData == nullptr) {
		LOG_WARNING("ResourceData have a null pointer.");
		return;
	}

	CModelBufferInfo* pModelBufferInfo = pResourceData->GetModelBufferInfo();

	uint32_t nVertexOffset = m_vecStaticVertices.size();
	uint32_t nIndexOffset = m_vecStaticIndices.size();

	pModelBufferInfo->nVertexOffset = nVertexOffset;
	pModelBufferInfo->nIndexOffset = nIndexOffset;

	pModelBufferInfo->nVertexCount = pResourceData->GetModelVertices().size();
	pModelBufferInfo->nIndexCount = pResourceData->GetModelIndices().size();
	
	m_vecStaticVertices.insert(
		m_vecStaticVertices.end(),
		pResourceData->GetModelVertices().begin(),
		pResourceData->GetModelVertices().end()
	);

	for (uint32_t nIndex : pResourceData->GetModelIndices()) {
		m_vecStaticIndices.push_back(nIndex + nVertexOffset);
	}

	this->UpdateBuffers();
}

bool CRenderer::LoadShadersFromFile() {
	ID3DBlob* pVertexShaderBlob = nullptr;
	ID3DBlob* pPixelShaderBlob = nullptr;

	ID3D11Device* pDevice = CGraphicsContext::GetInstance().GetDevice();

	HR(D3DReadFileToBlob(L"game/shaders/vertex_shader_c.cso", &pVertexShaderBlob));
	HR(D3DReadFileToBlob(L"game/shaders/pixel_shader_c.cso", &pPixelShaderBlob));
	if (!pVertexShaderBlob || !pPixelShaderBlob) {
		LOG_ERROR("Failed to initalize shaders.");
		return false;
	}

	HRESULT hResult = pDevice->CreateVertexShader(
		pVertexShaderBlob->GetBufferPointer(),
		pVertexShaderBlob->GetBufferSize(),
		nullptr,
		&m_pVertexShader
	);

	if (FAILED(hResult)) {
		LOG_ERROR("Failed to create vertex shader.");
		RELEASE_COM(pVertexShaderBlob);
		return false;
	}

	hResult = pDevice->CreatePixelShader(
		pPixelShaderBlob->GetBufferPointer(),
		pPixelShaderBlob->GetBufferSize(),
		nullptr,
		&m_pPixelShader
	);

	if (FAILED(hResult)) {
		LOG_ERROR("Failed to create pixel shader.");
		RELEASE_COM(pVertexShaderBlob);
		RELEASE_COM(pPixelShaderBlob);
		return false;
	}

	if (!CreateInputLayout(pVertexShaderBlob)) {
		LOG_ERROR("Failed to create input layout.");
		RELEASE_COM(pVertexShaderBlob);
		RELEASE_COM(pPixelShaderBlob);
		return false;
	}

	RELEASE_COM(pVertexShaderBlob);
	RELEASE_COM(pPixelShaderBlob);

	return true;
}

bool CRenderer::CreateInputLayout(ID3DBlob* pVSBlob) {
	D3D11_INPUT_ELEMENT_DESC VertexDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HR(CGraphicsContext::GetInstance().GetDevice()->
		CreateInputLayout(VertexDesc, 2, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pInputLayout));
	return true;
}

bool CRenderer::CreateWorldViewProjectionBuffer() {
	ID3D11Device* pDevice = CGraphicsContext::GetInstance().GetDevice();

	D3D11_BUFFER_DESC WVPBufferDesc = {};
	WVPBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	WVPBufferDesc.ByteWidth = sizeof(BufferPerObject_t);
	WVPBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	WVPBufferDesc.CPUAccessFlags = 0;
	WVPBufferDesc.MiscFlags = 0;

	HR(pDevice->CreateBuffer(&WVPBufferDesc, nullptr, &m_pWorldViewProjectionBuffer));
	return true;
}

void CRenderer::SetVertexBuffer(const std::vector<Vertex_t>& vecVertices) {
	// Unused
}

void CRenderer::SetIndexBuffer(const std::vector<uint32_t>& vecIndices) {
	// Unused
}

void CRenderer::UpdateBuffers() {
	ID3D11Device* pDevice = CGraphicsContext::GetInstance().GetDevice();

	D3D11_BUFFER_DESC VertexBufferDesc = {};
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.ByteWidth = sizeof(Vertex_t) * m_vecStaticVertices.size();
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA VertexBufferInitData = {};
	VertexBufferInitData.pSysMem = m_vecStaticVertices.data();
	RELEASE_COM(m_pStaticVertexBuffer);
	HR(pDevice->CreateBuffer(&VertexBufferDesc, &VertexBufferInitData, &m_pStaticVertexBuffer));

	D3D11_BUFFER_DESC IndexBufferDesc = {};
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.ByteWidth = sizeof(uint32_t) * m_vecStaticIndices.size();
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA IndexBufferInitData = {};
	IndexBufferInitData.pSysMem = m_vecStaticIndices.data();
	RELEASE_COM(m_pStaticIndexBuffer);
	HR(pDevice->CreateBuffer(&IndexBufferDesc, &IndexBufferInitData, &m_pStaticIndexBuffer));
}

void CRenderer::UpdateWorldViewProjectionBuffer() {
	ID3D11DeviceContext* pContext = CGraphicsContext::GetInstance().GetDeviceContext();
	// TODO: ...
}

CRenderer::~CRenderer() {

}
