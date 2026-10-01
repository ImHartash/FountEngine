#include "CRenderer.hpp"
#include "systems/CSystemManager.hpp"
#include "systems/logsystem/CLogSystem.hpp"
#include "systems/resourcesystem/CResourceSystem.hpp"
#include "systems/entitysystem/CEntitySystem.hpp"
#include "systems/filesystem/CFileSystem.hpp"
#include "engine/graphicscontext/CGraphicsContext.hpp"
#include "game/entities/basepropentity/CBasePropEntity.hpp"
#include "game/entities/light/directionallight/CDirectionalLight.hpp"
#include "game/entities/light/pointlight/CPointLight.hpp"
#include "game/entities/light/spotlight/CSpotLight.hpp"
#include "utils/defines.hpp"
#include "utils/BufferPerObject_t.hpp"
#include "utils/LightBuffer_t.hpp"
#include "enums/entity_flags.hpp"
#include "math/matrixutils/CMatrixUtils.hpp"
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

	if (!CreateGPUBuffers()) {
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

	CDirectionalLight* pKey = g_pEntitySystem->CreateEntity<CDirectionalLight>(
		Vector3_t(0.10f, 0.10f, 0.10f),   
		Vector3_t(0.90f, 0.90f, 0.90f), 
		Vector3_t(1.00f, 1.00f, 1.00f),   
		Vector3_t(-0.5f, -1.0f, -0.6f).Normalize()
	);

	CDirectionalLight* pFill = g_pEntitySystem->CreateEntity<CDirectionalLight>(
		Vector3_t(0.15f, 0.15f, 0.15f),
		Vector3_t(0.55f, 0.55f, 0.55f),
		Vector3_t(0.00f, 0.00f, 0.00f),
		Vector3_t(1.0f, -0.4f, -0.5f).Normalize()
	);

	CDirectionalLight* pRim = g_pEntitySystem->CreateEntity<CDirectionalLight>(
		Vector3_t(0.05f, 0.05f, 0.05f),
		Vector3_t(0.70f, 0.70f, 0.75f),
		Vector3_t(0.80f, 0.80f, 0.80f),
		Vector3_t(0.4f, -0.3f, 1.0f).Normalize()
	);

	CBasePropEntity* pEntity = g_pEntitySystem->CreateEntity<CBasePropEntity>(
		"models/character.fntmdl"
	);

	CBasePropEntity* pEntity2 = g_pEntitySystem->CreateEntity<CBasePropEntity>(
		"models/character2.fntmdl"
	);
	pEntity2->SetPosition({ 65.f, 0.f, 0.f });
	pEntity2->SetScale({ 3.5f, 3.5f, 3.5f });

	CBasePropEntity* pEntity3 = g_pEntitySystem->CreateEntity<CBasePropEntity>(
		"models/character3.fntmdl"
	);
	pEntity3->SetScale({ 8.f, 8.f, 8.f });
	pEntity3->SetRotation({ 0.f, DEG_TO_RAD(180.f), 0.f });
	pEntity3->SetPosition({ -65.f, 0.f, 0.f });
	// END OF TESTING

	m_PlayerCamera.SetPosition({ 0.f, 8.f, 135.f });
	m_PlayerCamera.SetSpeed(15.f);
	m_PlayerCamera.SetRotation({ 0.f, DEG_TO_RAD(180.f), 0.f });

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

	std::vector<CDirectionalLight*> vecDirLights;
	std::vector<CPointLight*> vecPointLights;
	std::vector<CSpotLight*> vecSpotLights;

	Vector3_t vecCameraPosition = m_PlayerCamera.GetPosition();

	for (EntitySlot_t* pEntitySlot = g_pEntitySystem->GetFirstSlot(); pEntitySlot; pEntitySlot = pEntitySlot->pNext) {
		IBaseEntity* pBase = pEntitySlot->pEntity.get();
		if (!pBase) continue;

		if (pEntitySlot->nFlags & ENT_FLAG_TYPE_RENDER_ENTITY) { // RENDER MODEL ENTITIES
			CBaseModelEntity* pModelEntity = static_cast<CBaseModelEntity*>(pBase);
			if (!pModelEntity) continue;

			CModelResourceData* pModel
				= g_pResourceSystem->GetResource<CModelResourceData>(pModelEntity->GetModelResource());
			if (!pModel) continue;

			for (int nIndex = 0; nIndex < pModel->GetSubmeshes().size(); nIndex++) {
				const ModelSubmesh_t& Submesh = pModel->GetSubmeshes()[nIndex];
				CMaterialResourceData* pMaterial
					= g_pResourceSystem->GetResource<CMaterialResourceData>(Submesh.hMaterial);
				if (!pMaterial) continue;

				RenderItem_t RenderItem{pModelEntity, pModel, pMaterial, nIndex, 0.f};

				if (pMaterial->GetBlendMode() == EMaterialBlendMode::Opaque) {
					m_vecOpaqueRenderList.push_back(RenderItem);
				}

				Vector3_t vecPositionDelta = pModelEntity->GetPosition() - vecCameraPosition;
				float flDistanceSq = vecPositionDelta.x * vecPositionDelta.x + vecPositionDelta.y * vecPositionDelta.y
					* vecPositionDelta.z * vecPositionDelta.z;
				//RenderItem.flDistanceSq = flDistanceSq;
				m_vecTransparentRenderList.push_back(RenderItem);
			}

			//CMaterialResourceData* pMaterial
			//	= g_pResourceSystem->GetResource<CMaterialResourceData>(pModelEntity->GetMaterialResource());
			//if (!pMaterial) continue;

			//if (pMaterial->GetBlendMode() == EMaterialBlendMode::Opaque) {
			//	m_vecOpaqueRenderList.push_back({ pModelEntity, pModel, pMaterial, 0.f });
			//	continue;
			//}

			//Vector3_t vecPositionDelta = pModelEntity->GetPosition() - vecCameraPosition;
			//float flDistanceSq = vecPositionDelta.x * vecPositionDelta.x + vecPositionDelta.y
			//	* vecPositionDelta.y + vecPositionDelta.z * vecPositionDelta.z;
			//m_vecTransparentRenderList.push_back({ pModelEntity, pModel, pMaterial, flDistanceSq });
		}
		if (pEntitySlot->nFlags & ENT_FLAG_TYPE_LIGHT_ENTITY) { // RENDER LIGHT ENTITIES
			if (pEntitySlot->nFlags & ENT_FLAG_LIGHT_DIR) {
				CDirectionalLight* pDirLightEntity = static_cast<CDirectionalLight*>(pBase);
				vecDirLights.push_back(pDirLightEntity);
			}
			if (pEntitySlot->nFlags & ENT_FLAG_LIGHT_POINT) {
				CPointLight* pPointLightEntity = static_cast<CPointLight*>(pBase);
				vecPointLights.push_back(pPointLightEntity);
			}
			if (pEntitySlot->nFlags & ENT_FLAG_LIGHT_SPOT) {
				CSpotLight* pSpotLightEntity = static_cast<CSpotLight*>(pBase);
				vecSpotLights.push_back(pSpotLightEntity);
			}
		}
	}

	std::sort(m_vecTransparentRenderList.begin(), m_vecTransparentRenderList.end(),
		[](const RenderItem_t& A, const RenderItem_t& B) {
			return A.flDistanceSq > B.flDistanceSq;
	});

	this->UpdateLightBuffer(vecDirLights, vecPointLights, vecSpotLights);
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

	pContext->PSSetConstantBuffers(1, 1, &m_pLightBuffer);

	pContext->IASetInputLayout(m_pInputLayout);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pContext->VSSetShader(m_pVertexShader, nullptr, 0);
	pContext->PSSetShader(m_pPixelShader, nullptr, 0);

	for (RenderItem_t& RenderEntity : m_vecOpaqueRenderList) {
		RenderModel(RenderEntity);
	}

	for (RenderItem_t& RenderEntity : m_vecTransparentRenderList) {
		RenderModel(RenderEntity);
	}
}

void CRenderer::RenderModel(const RenderItem_t& RenderItem) 
{
	ID3D11DeviceContext* pContext = CGraphicsContext::GetInstance().GetDeviceContext();
	//CGraphicsContext::GetInstance().ApplyMaterialStates(pEntityMaterial->GetBlendMode(),
	//	pEntityMaterial->GetCullMode(), pEntityMaterial->GetDepthMode());

	CGraphicsContext::GetInstance().ApplyMaterialStates(RenderItem.pMaterial->GetBlendMode(),
		RenderItem.pMaterial->GetCullMode(), RenderItem.pMaterial->GetDepthMode());

	const ModelSubmesh_t& Submesh = RenderItem.pModel->GetSubmeshes()[RenderItem.nSubmeshIndex];

	CResourceHandle hTexureHandle = RenderItem.pMaterial->GetDiffuseTexture();
	CTextureResourceData* pTextureData
		= g_pResourceSystem->GetResource<CTextureResourceData>(hTexureHandle);
	if (!pTextureData)
		return;

	if (m_GPUCache.find(RenderItem.pModel) == m_GPUCache.end()) {
		this->AddToStaticBuffers(RenderItem.pModel);
	}

	ModelGPUData_t& ModelBufferData = m_GPUCache[RenderItem.pModel];
	UpdateBufferPerObject(RenderItem.pEntity, RenderItem.pMaterial);

	ID3D11ShaderResourceView* pSRV = pTextureData->GetResourceView();

	pContext->VSSetConstantBuffers(0, 1, &m_pBufferPerObject);
	pContext->PSSetConstantBuffers(0, 1, &m_pBufferPerObject);
	pContext->PSSetShaderResources(0, 1, &pSRV);
	pContext->DrawIndexed(
		Submesh.nIndexCount,
		ModelBufferData.nIndexOffset + Submesh.nIndexOffset,
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
		m_vecStaticIndices.push_back(nIndex/* + nVertexOffset*/);
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

bool CRenderer::CreateGPUBuffers() {
	ID3D11Device* pDevice = CGraphicsContext::GetInstance().GetDevice();

	D3D11_BUFFER_DESC BufferPerObjectDesc = {};
	BufferPerObjectDesc.Usage = D3D11_USAGE_DEFAULT;
	BufferPerObjectDesc.ByteWidth = sizeof(BufferPerObject_t);
	BufferPerObjectDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferPerObjectDesc.CPUAccessFlags = 0;
	BufferPerObjectDesc.MiscFlags = 0;

	HR(pDevice->CreateBuffer(&BufferPerObjectDesc, nullptr, &m_pBufferPerObject));
	if (!m_pBufferPerObject) return false;

	D3D11_BUFFER_DESC LightBufferDesc = {};
	LightBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	LightBufferDesc.ByteWidth = sizeof(LightBuffer_t);
	LightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	LightBufferDesc.CPUAccessFlags = 0;
	LightBufferDesc.MiscFlags = 0;

	HR(pDevice->CreateBuffer(&LightBufferDesc, nullptr, &m_pLightBuffer));
	if (!m_pLightBuffer) return false;

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
	DirectX::XMMATRIX mtWorldInvTranspose = CMatrixUtils::InverseTranspose(mtWorld);

	DirectX::XMMATRIX mtView = m_PlayerCamera.GetViewMatrix();
	DirectX::XMMATRIX mtProjection = DirectX::XMLoadFloat4x4(&CGraphicsContext::GetInstance().GetProjectionMatrix());
	DirectX::XMMATRIX mtWorldViewProjection = DirectX::XMMatrixTranspose(mtWorld * mtView * mtProjection);

	DirectX::XMStoreFloat4x4(&BufferPerObject.mtWorld, DirectX::XMMatrixTranspose(mtWorld));
	DirectX::XMStoreFloat4x4(&BufferPerObject.mtWorldInverseTranspose, DirectX::XMMatrixTranspose(mtWorldInvTranspose));
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

void CRenderer::UpdateLightBuffer(const std::vector<CDirectionalLight*>& vecDirectionalLights, 
	const std::vector<CPointLight*>& vecPointLights, const std::vector<CSpotLight*>& vecSpotLights) {
	ID3D11DeviceContext* pContext = CGraphicsContext::GetInstance().GetDeviceContext();
	LightBuffer_t LightBuffer;

	LightBuffer.nDirectionalLightsNumber = 0;
	LightBuffer.nPointLightsNumber = 0;
	LightBuffer.nSpotLightsNumber = 0;

	LightBuffer.vec3CameraPositionWorld = m_PlayerCamera.GetPosition().DXAsFloat3();

	LightBuffer.FogData.nFogEnabled = 0;
	LightBuffer.FogData.flFogStart = 10.f;
	LightBuffer.FogData.flFogRange = 10.f;
	LightBuffer.FogData.vec3FogColor = { 1.f, 1.f, 1.f };

	for (int nIndex = 0; nIndex < vecDirectionalLights.size(); nIndex += 1) {
		if (nIndex >= NUM_MAX_DIR_LIGHTS) break;

		CDirectionalLight* pLight = vecDirectionalLights[nIndex];
		DirectionalLightData_t LightData = {
			pLight->GetAmbient().DXAsFloat3(), 0,
			pLight->GetDiffuse().DXAsFloat3(), 0,
			pLight->GetSpecular().DXAsFloat3(), 0,

			pLight->GetDirection().DXAsFloat3(), 0
		};

		LightBuffer.arrDirectionalLights[nIndex] = LightData;
		LightBuffer.nDirectionalLightsNumber += 1;
	}

	for (int nIndex = 0; nIndex < vecPointLights.size(); nIndex += 1) {
		if (nIndex >= NUM_MAX_POINT_LIGHTS) break;

		CPointLight* pLight = vecPointLights[nIndex];
		PointLightData_t LightData = {
			pLight->GetAmbient().DXAsFloat3(), 0,
			pLight->GetDiffuse().DXAsFloat3(), 0,
			pLight->GetSpecular().DXAsFloat3(), 0,

			pLight->GetPosition().DXAsFloat3(),
			pLight->GetRange(),
			pLight->GetAttenuation().DXAsFloat3(), 0
		};

		LightBuffer.arrPointLights[nIndex] = LightData;
		LightBuffer.nPointLightsNumber += 1;
	}

	for (int nIndex = 0; nIndex < vecSpotLights.size(); nIndex += 1) {
		if (nIndex >= NUM_MAX_SPOT_LIGHTS) break;

		CSpotLight* pLight = vecSpotLights[nIndex];
		SpotLightData_t LightData = {
			pLight->GetAmbient().DXAsFloat3(), 0,
			pLight->GetDiffuse().DXAsFloat3(), 0,
			pLight->GetSpecular().DXAsFloat3(), 0,

			pLight->GetPosition().DXAsFloat3(),
			pLight->GetRange(),
			pLight->GetDirection().DXAsFloat3(),
			pLight->GetSpot(),
			pLight->GetAttenuation().DXAsFloat3(),
			0
		};

		LightBuffer.arrSpotLights[nIndex] = LightData;
		LightBuffer.nSpotLightsNumber += 1;
	}

	pContext->UpdateSubresource(
		m_pLightBuffer,
		0, nullptr,
		&LightBuffer,
		0, 0
	);
}

DirectX::XMMATRIX CRenderer::GetWorldMatrixFromObject(CBaseModelEntity* pModelEntity) {
	Vector3_t& vecPosition = pModelEntity->GetPosition();
	Vector3_t& vecScaling = pModelEntity->GetScale();
	Angle_t& vecViewAngle = pModelEntity->GetViewAngle();

	DirectX::XMMATRIX mtScalingMatrix = DirectX::XMMatrixScaling(vecScaling.x, vecScaling.y, vecScaling.z);
	DirectX::XMMATRIX mtRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(vecViewAngle.flPitch, vecViewAngle.flYaw, vecViewAngle.flRoll);
	DirectX::XMMATRIX mtTranslationMatrix = DirectX::XMMatrixTranslation(vecPosition.x, vecPosition.y, vecPosition.z);

	return mtScalingMatrix * mtRotationMatrix * mtTranslationMatrix;
}

CRenderer::CRenderer() 
	: m_pVertexShader(nullptr), m_pPixelShader(nullptr), m_pStaticVertexBuffer(nullptr),
	m_pStaticIndexBuffer(nullptr), m_pBufferPerObject(nullptr), m_pLightBuffer(nullptr),
	m_pInputLayout(nullptr), m_pTextureSampler(nullptr) {}

CRenderer::~CRenderer() {
	RELEASE_COM(m_pVertexShader);
	RELEASE_COM(m_pPixelShader);
	RELEASE_COM(m_pStaticVertexBuffer);
	RELEASE_COM(m_pStaticIndexBuffer);
	RELEASE_COM(m_pBufferPerObject);
	RELEASE_COM(m_pInputLayout);
	RELEASE_COM(m_pTextureSampler);
}
