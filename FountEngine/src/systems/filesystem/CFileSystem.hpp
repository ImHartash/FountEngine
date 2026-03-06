#pragma once
#include <string>

class CFileSystem {
public:
	static CFileSystem& GetInstance();

	void Initialize();
	std::string GetRelativePath(const std::string& strResourcePath);
	std::string GetExtension(const std::string& strResourcePath);

private:
	std::string m_strResourcePath;

	CFileSystem() = default;
	~CFileSystem() = default;
};