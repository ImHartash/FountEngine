#pragma once
#include <vector>
#include "math/vertex.hpp"
#include "../IResource.hpp"

class CModelResourceData : public IResource {
public:
	CModelResourceData() = default;
	CModelResourceData(std::vector<Vertex_t> vecVertices, std::vector<uint32_t> vecIndices)
		: m_vecVertices(std::move(vecVertices)), m_vecIndices(std::move(vecIndices)) { }

	const std::vector<Vertex_t>& GetVertices() { return m_vecVertices; }
	const std::vector<uint32_t>& GetIndices() { return m_vecIndices; }

private:
	std::vector<Vertex_t> m_vecVertices;
	std::vector<uint32_t> m_vecIndices;
};