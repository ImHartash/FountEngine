#include "CFntPackFile.hpp"
#include "../headers/fntpk_header.hpp"
#include "../headers/fntpk_block.hpp"

bool CFntPackFile::Load(const std::string& strPath) {
    m_File.open(strPath, std::ios::binary);
    if (!m_File.is_open()) return false;

    FNTPK_HEADER Header = {};
    m_File.read(reinterpret_cast<char*>(&Header), sizeof(FNTPK_HEADER));

    if (Header.nMagic != 0x50544E46)
        return false;

    std::vector<FNTPK_BLOCK> vecBlocks(Header.nBlocksCount);
    m_File.read(reinterpret_cast<char*>(vecBlocks.data()), vecBlocks.size() * sizeof(FNTPK_BLOCK));

    for (auto& Resource : vecBlocks) {
        FntPakEntry_t Entry;
        Entry.strName = Resource.szResourceName;
        Entry.nOffset = Resource.nBlockDataOffset;
        Entry.nSize = Resource.nBlockDataSize;

        m_Entries[Entry.strName] = Entry;
    }

    return true;
}

bool CFntPackFile::ReadFile(const std::string& strResourceName, std::vector<char>& vecDataBuffer) {
    auto It = m_Entries.find(strResourceName);
    if (It == m_Entries.end()) return false;

    const auto& Entry = It->second;

    vecDataBuffer.resize(Entry.nSize);

    m_File.seekg(Entry.nOffset);
    m_File.read(vecDataBuffer.data(), Entry.nSize);

    return true;
}
