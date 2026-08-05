#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <unordered_map>
#include "game/resource/modelresourcedata/CModelResourceData.hpp"
#include "game/resource/textureresourcedata/CTextureResourceData.hpp"
#include "game/resource/materialresourcedata/CMaterialResourceData.hpp"
#include "game/resource/modelgpudata/ModelGPUData_t.hpp"
#include "game/entitites/basemodelentity/CBaseModelEntity.hpp"
#include "game/camera/CCameraObject.hpp"

class CDirectionalLight;
class CPointLight;
class CSpotLight;

struct RenderItem_t {
	CBaseModelEntity* pEntity;
	CModelResourceData* pModel;
	CMaterialResourceData* pMaterial;
	float flDistanceSq;
};

class CRenderer {
public:
	static CRenderer& GetInstance();

	bool Initialize();

	void UpdateSceneComponents(float flDeltaTime);
	void PrepareFrame();
	void RenderScene();
	void RenderModel(CBaseModelEntity* pModelEntity,
		CModelResourceData* pEntityModel, CMaterialResourceData* pEntityMaterial);

	void AddToStaticBuffers(CModelResourceData* pResourceData);

private:
	bool LoadShadersFromFile();
	bool CreateInputLayout(ID3DBlob* pVSBlob);
	bool CreateGPUBuffers();
	bool CreateTextureSampler();

	void SetVertexBuffer(const std::vector<Vertex_t>& vecVertices);
	void SetIndexBuffer(const std::vector<uint32_t>& vecIndices);
	void UpdateBuffers();

	// Update buffers
	void UpdateBufferPerObject(CBaseModelEntity* pModelEntity, CMaterialResourceData* pMaterial);
	void UpdateLightBuffer(const std::vector<CDirectionalLight*>& vecDirectionalLights,
		const std::vector<CPointLight*>& vecPointLights, const std::vector<CSpotLight*>& vecSpotLights);

	DirectX::XMMATRIX GetWorldMatrixFromObject(CBaseModelEntity* pModelEntity);

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;

	std::vector<Vertex_t> m_vecStaticVertices;
	std::vector<uint32_t> m_vecStaticIndices;

	std::vector<RenderItem_t> m_vecOpaqueRenderList;
	std::vector<RenderItem_t> m_vecTransparentRenderList;

	ID3D11Buffer* m_pStaticVertexBuffer;
	ID3D11Buffer* m_pStaticIndexBuffer;
	ID3D11Buffer* m_pBufferPerObject;
	ID3D11Buffer* m_pLightBuffer;

	ID3D11InputLayout* m_pInputLayout;
	ID3D11SamplerState* m_pTextureSampler;

	CCameraObject m_PlayerCamera;
	std::unordered_map<CModelResourceData*, ModelGPUData_t> m_GPUCache;

	CRenderer();
	~CRenderer();
};