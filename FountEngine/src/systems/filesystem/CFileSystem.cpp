#include "CFileSystem.hpp"
#include <filesystem>
#include "systems/logging/CLogSystem.hpp"

namespace fs = std::filesystem;

CFileSystem& CFileSystem::GetInstance() {
	static CFileSystem Instance;
	return Instance;
}

void CFileSystem::Initialize() {
	std::filesystem::create_directory("game/resources");
	m_strResourcePath = "game/resources/";
}

void CFileSystem::MountPakFile(const std::string& strPackPath) {
	CFntPackFile PackFile;

	fs::path FullPath = m_strResourcePath;
	FullPath /= strPackPath;

	if (PackFile.Load(FullPath.string())) {
		m_FntPacks.push_back(std::move(PackFile));
	}
	else { LOG_WARNING("Failed to load .fntpk file %s", strPackPath.c_str()); }
}

bool CFileSystem::ReadFile(const std::string& strFilePath, std::vector<char>& vecBufferData) {
	for (auto& Pack : m_FntPacks) {
		if (Pack.ReadFile(strFilePath, vecBufferData))
			return true;
	}

	fs::path FullPath = m_strResourcePath;
	FullPath /= strFilePath;

	std::ifstream fin(FullPath, std::ios::binary | std::ios::ate);
	if (!fin.is_open())
		return false;

	size_t nSize = fin.tellg();
	fin.seekg(0);

	vecBufferData.resize(nSize);
	fin.read(vecBufferData.data(), nSize);

	return true;
}
