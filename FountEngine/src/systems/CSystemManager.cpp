#include "CSystemManager.hpp"
#include "entitysystem/CEntitySystem.hpp"
#include "filesystem/CFileSystem.hpp"
#include "inputsystem/CInputSystem.hpp"
#include "logsystem/CLogSystem.hpp"
#include "resourcesystem/CResourceSystem.hpp"

CEntitySystem* g_pEntitySystem = nullptr;
CFileSystem* g_pFileSystem = nullptr;
CInputSystem* g_pInputSystem = nullptr;
CLogSystem* g_pLogSystem = nullptr;
CResourceSystem* g_pResourceSystem = nullptr;

CSystemManager& CSystemManager::GetInstance() {
    static CSystemManager Instance;
    return Instance;
}

bool CSystemManager::Initialize() {
    bool m_bInitialized = true;

	m_pEntitySystem = std::make_unique<CEntitySystem>();
    g_pEntitySystem = m_pEntitySystem.get();

    m_pFileSystem = std::make_unique<CFileSystem>();
    g_pFileSystem = m_pFileSystem.get();

    m_pInputSystem = std::make_unique<CInputSystem>();
    g_pInputSystem = m_pInputSystem.get();

    m_pLogSystem = std::make_unique<CLogSystem>();
    g_pLogSystem = m_pLogSystem.get();

    m_pResourceSystem = std::make_unique<CResourceSystem>();
    g_pResourceSystem = m_pResourceSystem.get();

    return m_bInitialized;
}

void CSystemManager::Shutdown() {
    m_pEntitySystem.reset();
    m_pFileSystem.reset();
    m_pInputSystem.reset();
    m_pLogSystem.reset();
    m_pResourceSystem.reset();

    g_pEntitySystem = nullptr;
    g_pFileSystem = nullptr;
    g_pInputSystem = nullptr;
    g_pLogSystem = nullptr;
    g_pResourceSystem = nullptr;
}