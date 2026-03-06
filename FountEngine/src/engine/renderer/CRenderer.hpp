#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include "game/resource/modelresourcedata/CModelResourceData.hpp"
#include "game/camera/CCameraObject.hpp"

class CRenderer {
public:
	static CRenderer& GetInstance();

	bool Initialize();

	void UpdateSceneComponents(float flDeltaTime);
	void RenderScene();
	void RenderModel(CModelResourceData* pModelResource, DirectX::XMMATRIX mtWorldMatrix);

	void AddToStaticBuffers(CModelResourceData* pResourceData);

private:
	bool LoadShadersFromFile();
	bool CreateInputLayout(ID3DBlob* pVSBlob);
	bool CreateWorldViewProjectionBuffer();

	void SetVertexBuffer(const std::vector<Vertex_t>& vecVertices);
	void SetIndexBuffer(const std::vector<uint32_t>& vecIndices);
	void UpdateBuffers();

	void UpdateWorldViewProjectionBuffer();

	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;

	std::vector<Vertex_t> m_vecStaticVertices;
	std::vector<uint32_t> m_vecStaticIndices;

	ID3D11Buffer* m_pStaticVertexBuffer;
	ID3D11Buffer* m_pStaticIndexBuffer;
	ID3D11Buffer* m_pWorldViewProjectionBuffer;

	ID3D11InputLayout* m_pInputLayout;

	CCameraObject m_PlayerCamera;

	CRenderer();
	~CRenderer();
};