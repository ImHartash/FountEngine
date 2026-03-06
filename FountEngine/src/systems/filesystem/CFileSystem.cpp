#include "CFileSystem.hpp"
#include <filesystem>

CFileSystem& CFileSystem::GetInstance() {
	static CFileSystem Instance;
	return Instance;
}

void CFileSystem::Initialize() {
	std::filesystem::create_directory("game/resources");
	m_strResourcePath = "game/resources/";
}

std::string CFileSystem::GetRelativePath(const std::string& strResourcePath) {
	return m_strResourcePath + strResourcePath;
}

std::string CFileSystem::GetExtension(const std::string& strResourcePath) {
	size_t nDotPos = strResourcePath.find_last_of('.');
	if (nDotPos != std::string::npos && nDotPos < strResourcePath.length() - 1)
		return strResourcePath.substr(nDotPos);
	return "";
}
