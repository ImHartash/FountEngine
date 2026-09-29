#pragma once
#include <vector>
#include "math/vertex.hpp"
#include "../IResource.hpp"
#include "../CResourceHandle.hpp"

struct ModelSubmesh_t {
	uint32_t nIndexOffset = 0;
	uint32_t nIndexCount = 0;
	CResourceHandle hMaterial;
};

class CModelResourceData : public IResource {
public:
	CModelResourceData() = default;
	CModelResourceData(std::vector<Vertex_t> vecVertices, std::vector<uint32_t> vecIndices, std::vector<ModelSubmesh_t> vecSubmeshes)
		: m_vecVertices(std::move(vecVertices)), m_vecIndices(std::move(vecIndices)), m_vecSubmeshes(std::move(vecSubmeshes)) { }

	const std::vector<Vertex_t>& GetVertices() { return m_vecVertices; }
	const std::vector<uint32_t>& GetIndices() { return m_vecIndices; }
	const std::vector<ModelSubmesh_t>& GetSubmeshes() { return m_vecSubmeshes; }

private:
	std::vector<Vertex_t> m_vecVertices;
	std::vector<uint32_t> m_vecIndices;
	std::vector<ModelSubmesh_t> m_vecSubmeshes;
};