#include "CRenderer.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/logsystem/CLogSystem.hpp"
#include "systems/resourcesystem/CResourceSystem.hpp"
#include "systems/entitysystem/CEntitySystem.hpp"
#include "systems/filesystem/CFileSystem.hpp"
#include "engine/graphicscontext/CGraphicsContext.hpp"
#include "game/entitites/cubeentity/CCubeEntity.hpp"
#include "utils/defines.hpp"
#include "utils/BufferPerObject_t.hpp"
#include <algorithm>

CRenderer& CRenderer::GetInstance() {
	static CRenderer Instance;
	return Instance;
}

bool CRenderer::Initialize() {
	if (!LoadShadersFromFile()) {
		LOG_ERROR("Failed to load shaders.");
		return false;
	}

	if (!CreateBufferPerObject()) {
		LOG_ERROR("Failed to create World View Projection buffer.");
		return false;
	}

	if (!CreateTextureSampler()) {
		LOG_ERROR("Failed to create TextureSampler.");
		return false;
	}

	// !!! ONLY FOR TEST !!!
	// Make this on scene, not here :DD
	g_pFileSystem->MountPakFile("fountpak01.fntpk");
	g_pEntitySystem->CreateEntity<CCubeEntity>();
	CCubeEntity* pEntity = g_pEntitySystem->CreateEntity<CCubeEntity>();
	pEntity->SetPosition({4.f, 0.f, 0.f});
	pEntity->SetMaterialResource("materials/test_blend.fntmat");

	m_PlayerCamera.SetPosition({ 0.f, 0.f, -5.f });

	LOG_INFO("Renderer successfully initialized.");
	return true;
}

void CRenderer::UpdateSceneComponents(float flDeltaTime) {
	m_PlayerCamera.Update(flDeltaTime);
	g_pEntitySystem->UpdateAllEntities(flDeltaTime);
}

void CRenderer::PrepareFrame() {
	m_vecOpaqueRenderList.clear();
	m_vecTransparentRenderList.clear();

	Vector3_t vecCameraPosition = m_PlayerCamera.GetPosition();

	for (EntitySlot_t* pEntitySlot = g_pEntitySystem->GetFirstSlot(); pEntitySlot; pEntitySlot = pEntitySlot->pNext) {
		IBaseEntity* pBase = pEntitySlot->pEntity.get();
		if (!pBase) continue;

		CBaseModelEntity* pModelEntity = dynamic_cast<CBaseModelEntity*>(pBase);
		if (!pModelEntity) continue;

		CModelResourceData* pModel
			= g_pResourceSystem->GetResource<CModelResourceData>(pModelEntity->GetModelResource());
		if (!pModel) continue;

		CMaterialResourceData* pMaterial 
			= g_pResourceSystem->GetResource<CMaterialResourceData>(pModelEntity->GetMaterialResource());
		if (!pMaterial) continue;

		if (pMaterial->GetBlendMode() == EMaterialBlendMode::Opaque) {
			m_vecOpaqueRenderList.push_back({ pModelEntity, pModel, pMaterial, 0.f });
			continue;
		}

		Vector3_t vecPositionDelta = pModelEntity->GetPosition() - vecCameraPosition;
		float flDistanceSq = vecPositionDelta.x * vecPositionDelta.x + vecPositionDelta.y 
			* vecPositionDelta.y + vecPositionDelta.z * vecPositionDelta.z;
		m_vecTransparentRenderList.push_back({ pModelEntity, pModel, pMaterial, flDistanceSq });
	}

	std::sort(m_vecTransparentRenderList.begin(), m_vecTransparentRenderList.end(),
		[](const RenderItem_t& A, const RenderItem_t& B) {
			return A.flDistanceSq > B.flDistanceSq;
		});
}

void CRenderer::RenderScene() {
	ID3D11DeviceContext* pContext = CGraphicsContext::GetInstance().GetDeviceContext();
	UINT nStride = sizeof(Vertex_t);
	UINT nOffset = 0;

	pContext->IASetVertexBuffers(
		0,
		1,
		&m_pStaticVertexBuffer,
		&nStride,
		&nOffset
	);

	pContext->IASetIndexBuffer(
		m_pStaticIndexBuffer,
		DXGI_FORMAT_R32_UINT,
		0
	);

	pContext->IASetInputLayout(m_pInputLayout);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pContext->PSSetShader(m_pPixelShader, nullptr, 0);

	for (RenderItem_t& RenderEntity : m_vecOpaqueRenderList) {
		RenderModel(RenderEntity.pEntity, RenderEntity.pModel, RenderEntity.pMaterial);
	}

	for (RenderItem_t& RenderEntity : m_vecTransparentRenderList) {
		RenderModel(RenderEntity.pEntity, RenderEntity.pModel, RenderEntity.pMaterial);
	}

	/*for (uint32_t nEntityIndex = 0; nEntityIndex < CEntitySystem::GetInstance().GetMaxIndex(); nEntityIndex++) {
		CBaseModelEntity* pModelEntity = CEntitySystem::GetInstance().GetEntityByIndex<CBaseModelEntity>(nEntityIndex);
		if (pModelEntity == nullptr)
			continue;

		RenderModel(pModelEntity);
	}*/
}

void CRenderer::RenderModel(CBaseModelEntity* pModelEntity, 
	CModelResourceData* pEntityModel, CMaterialResourceData* pEntityMaterial) 
{
	ID3D11DeviceContext* pContext = CGraphicsContext::GetInstance().GetDeviceContext();
	CGraphicsContext::GetInstance().ApplyMaterialStates(pEntityMaterial->GetBlendMode(),
		pEntityMaterial->GetCullMode(), pEntityMaterial->GetDepthMode());

	CResourceHandle hTexureHandle = pEntityMaterial->GetDiffuseTexture();
	CTextureResourceData* pTextureData
		= g_pResourceSystem->GetResource<CTextureResourceData>(hTexureHandle);
	if (pTextureData == nullptr)
		return;

	if (m_GPUCache.find(pEntityModel) == m_GPUCache.end()) {
		this->AddToStaticBuffers(pEntityModel);
	}

	ModelGPUData_t& ModelBufferData = m_GPUCache[pEntityModel];
	UpdateBufferPerObject(pModelEntity, pEntityMaterial);

	ID3D11ShaderResourceView* pSRV = pTextureData->GetResourceView();

	pContext->VSSetConstantBuffers(0, 1, &m_pBufferPerObject);
	pContext->PSSetConstantBuffers(0, 1, &m_pBufferPerObject);
	pContext->PSSetShaderResources(0, 1, &pSRV);
	pContext->DrawIndexed(
		ModelBufferData.nIndexCount,
		ModelBufferData.nIndexOffset,
		ModelBufferData.nVertexOffset
	);
}

void CRenderer::AddToStaticBuffers(CModelResourceData* pResourceData) {
	if (pResourceData == nullptr) {
		LOG_WARNING("ResourceData have a null pointer.");
		return;
	}

	ModelGPUData_t ModelBufferInfo = {};

	uint32_t nVertexOffset = static_cast<uint32_t>(m_vecStaticVertices.size());
	uint32_t nIndexOffset = static_cast<uint32_t>(m_vecStaticIndices.size());

	ModelBufferInfo.nVertexOffset = nVertexOffset;
	ModelBufferInfo.nIndexOffset = nIndexOffset;

	/*ModelBufferInfo.nVertexCount = static_cast<uint32_t>(pResourceData->GetModelVertices().size());*/
	ModelBufferInfo.nIndexCount = static_cast<uint32_t>(pResourceData->GetIndices().size());

	m_vecStaticVertices.insert(
		m_vecStaticVertices.end(),
		pResourceData->GetVertices().begin(),
		pResourceData->GetVertices().end()
	);

	for (uint32_t nIndex : pResourceData->GetIndices()) {
		m_vecStaticIndices.push_back(nIndex + nVertexOffset);
	}

	m_GPUCache[pResourceData] = ModelBufferInfo;
	this->UpdateBuffers();
}

bool CRenderer::LoadShadersFromFile() {
	ID3D11DeviceContext* pContext = CGraphicsContext::GetInstance().GetDeviceContext();
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

	pContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pContext->PSSetShader(m_pPixelShader, nullptr, 0);

	return true;
}

bool CRenderer::CreateInputLayout(ID3DBlob* pVSBlob) {
	D3D11_INPUT_ELEMENT_DESC VertexDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HR(CGraphicsContext::GetInstance().GetDevice()->
		CreateInputLayout(VertexDesc, 3, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pInputLayout));
	return true;
}

bool CRenderer::CreateBufferPerObject() {
	ID3D11Device* pDevice = CGraphicsContext::GetInstance().GetDevice();

	D3D11_BUFFER_DESC WVPBufferDesc = {};
	WVPBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	WVPBufferDesc.ByteWidth = sizeof(BufferPerObject_t);
	WVPBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	WVPBufferDesc.CPUAccessFlags = 0;
	WVPBufferDesc.MiscFlags = 0;

	HR(pDevice->CreateBuffer(&WVPBufferDesc, nullptr, &m_pBufferPerObject));
	return true;
}

bool CRenderer::CreateTextureSampler() {
	ID3D11Device* pDevice = CGraphicsContext::GetInstance().GetDevice();
	ID3D11DeviceContext* pDeviceContext = CGraphicsContext::GetInstance().GetDeviceContext();

	// Anisotropic Sampler. Feel free to change config..
	D3D11_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	SamplerDesc.MaxAnisotropy = 4;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	HR(pDevice->CreateSamplerState(&SamplerDesc, &m_pTextureSampler));
	pDeviceContext->PSSetSamplers(0, 1, &m_pTextureSampler);
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
	VertexBufferDesc.ByteWidth = static_cast <UINT>(sizeof(Vertex_t) * m_vecStaticVertices.size());
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA VertexBufferInitData = {};
	VertexBufferInitData.pSysMem = m_vecStaticVertices.data();
	RELEASE_COM(m_pStaticVertexBuffer);
	HR(pDevice->CreateBuffer(&VertexBufferDesc, &VertexBufferInitData, &m_pStaticVertexBuffer));

	D3D11_BUFFER_DESC IndexBufferDesc = {};
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.ByteWidth = static_cast <UINT>(sizeof(uint32_t) * m_vecStaticIndices.size());
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA IndexBufferInitData = {};
	IndexBufferInitData.pSysMem = m_vecStaticIndices.data();
	RELEASE_COM(m_pStaticIndexBuffer);
	HR(pDevice->CreateBuffer(&IndexBufferDesc, &IndexBufferInitData, &m_pStaticIndexBuffer));
}

void CRenderer::UpdateBufferPerObject(CBaseModelEntity* pModelEntity, CMaterialResourceData* pMaterial) {
	ID3D11DeviceContext* pContext = CGraphicsContext::GetInstance().GetDeviceContext();
	BufferPerObject_t BufferPerObject;

	DirectX::XMMATRIX mtWorld = GetWorldMatrixFromObject(pModelEntity);
	DirectX::XMMATRIX mtView = m_PlayerCamera.GetViewMatrix();
	DirectX::XMMATRIX mtProjection = DirectX::XMLoadFloat4x4(&CGraphicsContext::GetInstance().GetProjectionMatrix());
	DirectX::XMMATRIX mtWorldViewProjection = DirectX::XMMatrixTranspose(mtWorld * mtView * mtProjection);

	DirectX::XMStoreFloat4x4(&BufferPerObject.mtWorldViewProjection, mtWorldViewProjection);

	const Vector3_t& vecAmbient = pMaterial->GetAmbient();
	const Vector3_t& vecDiffuse = pMaterial->GetDiffuse();
	const Vector3_t& vecSpecular = pMaterial->GetSpecular();

	BufferPerObject.Material.vec3Ambient = { vecAmbient.x, vecAmbient.y, vecAmbient.z };
	BufferPerObject.Material.flShininess = pMaterial->GetShininess();
	BufferPerObject.Material.vec3Diffuse = { vecDiffuse.x, vecDiffuse.y, vecDiffuse.z };
	BufferPerObject.Material.flOpacity = pMaterial->GetOpacity();
	BufferPerObject.Material.vec3Specular = { vecSpecular.x, vecSpecular.y, vecSpecular.z };
	BufferPerObject.Material._flPad0 = 0.f;

	pContext->UpdateSubresource(
		m_pBufferPerObject,
		0, nullptr,
		&BufferPerObject,
		0, 0
	);
}

DirectX::XMMATRIX CRenderer::GetWorldMatrixFromObject(CBaseModelEntity* pModelEntity) {
	Vector3_t& vecPosition = pModelEntity->GetPosition();
	Angle_t& vecViewAngle = pModelEntity->GetViewAngle();

	DirectX::XMMATRIX mtScalingMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX mtRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(vecViewAngle.flPitch, vecViewAngle.flYaw, vecViewAngle.flRoll);
	DirectX::XMMATRIX mtTranslationMatrix = DirectX::XMMatrixTranslation(vecPosition.x, vecPosition.y, vecPosition.z);

	return mtScalingMatrix * mtRotationMatrix * mtTranslationMatrix;
}

CRenderer::CRenderer() 
	: m_pVertexShader(nullptr), m_pPixelShader(nullptr), m_pStaticVertexBuffer(nullptr),
	m_pStaticIndexBuffer(nullptr), m_pBufferPerObject(nullptr), m_pInputLayout(nullptr),
	m_pTextureSampler(nullptr) {}

CRenderer::~CRenderer() {
	RELEASE_COM(m_pVertexShader);
	RELEASE_COM(m_pPixelShader);
	RELEASE_COM(m_pStaticVertexBuffer);
	RELEASE_COM(m_pStaticIndexBuffer);
	RELEASE_COM(m_pBufferPerObject);
	RELEASE_COM(m_pInputLayout);
	RELEASE_COM(m_pTextureSampler);
}
