#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <unordered_map>
#include "game/resource/modelresourcedata/CModelResourceData.hpp"
#include "game/resource/textureresourcedata/CTextureResourceData.hpp"
#include "game/resource/modelgpudata/ModelGPUData_t.hpp"
#include "game/entitites/basemodelentity/CBaseModelEntity.hpp"
#include "game/camera/CCameraObject.hpp"

class CRenderer {
public:
	static CRenderer& GetInstance();

	bool Initialize();

	void UpdateSceneComponents(float flDeltaTime);
	void RenderScene();
	void RenderModel(CBaseModelEntity* pModelResource);

	void AddToStaticBuffers(CModelResourceData* pResourceData);

private:
	bool LoadShadersFromFile();
	bool CreateInputLayout(ID3DBlob* pVSBlob);
	bool CreateWorldViewProjectionBuffer();
	bool CreateTextureSampler();

	void SetVertexBuffer(const std::vector<Vertex_t>& vecVertices);
	void SetIndexBuffer(const std::vector<uint32_t>& vecIndices);
	void UpdateBuffers();

	void UpdateWorldViewProjectionBuffer(CBaseModelEntity* pModelEntity);
	DirectX::XMMATRIX GetWorldMatrixFromObject(CBaseModelEntity* pModelEntity);

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;

	std::vector<Vertex_t> m_vecStaticVertices;
	std::vector<uint32_t> m_vecStaticIndices;

	ID3D11Buffer* m_pStaticVertexBuffer;
	ID3D11Buffer* m_pStaticIndexBuffer;
	ID3D11Buffer* m_pWorldViewProjectionBuffer;

	ID3D11InputLayout* m_pInputLayout;
	ID3D11SamplerState* m_pTextureSampler;

	CCameraObject m_PlayerCamera;
	std::unordered_map<CModelResourceData*, ModelGPUData_t> m_GPUCache;

	CRenderer();
	~CRenderer();
};