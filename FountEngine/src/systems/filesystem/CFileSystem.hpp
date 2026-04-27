#pragma once
#include <string>
#include "fntpk/CFntPackFile.hpp"

class CFileSystem {
public:
	static CFileSystem& GetInstance();

	void Initialize();

	void MountPakFile(const std::string& strPakPath);
	bool ReadFile(const std::string& strFilePath, std::vector<char>& vecBufferData);

private:
	std::vector<CFntPackFile> m_FntPacks;
	std::string m_strResourcePath;

	CFileSystem() = default;
	~CFileSystem() = default;
};