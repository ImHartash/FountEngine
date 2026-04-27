#pragma once
#include <unordered_map>
#include <fstream>

struct FntPakEntry_t {
	std::string strName;
	uint64_t nOffset;
	uint32_t nSize;
};

class CFntPackFile {
public:
	bool Load(const std::string& strPath);
	bool ReadFile(const std::string& strResourceName, std::vector<char>& vecDataBuffer);

private:
	std::ifstream m_File;
	std::unordered_map<std::string, FntPakEntry_t> m_Entries;
};