#pragma once
#include <vector>
#include <memory>
#include "math/vertex.hpp"

class CModelResourceData {
public:
	CModelResourceData() = default;
	CModelResourceData(std::vector<Vertex_t> vecVertices, std::vector<uint32_t> vecIndices) 
		: m_vecModelVertices(std::move(vecVertices)), m_vecModelIndices(std::move(vecIndices)) { }

	std::vector<Vertex_t>& GetModelVertices() { return m_vecModelVertices; }
	std::vector<uint32_t>& GetModelIndices() { return m_vecModelIndices; }

private:
	std::vector<Vertex_t> m_vecModelVertices;
	std::vector<uint32_t> m_vecModelIndices;
};