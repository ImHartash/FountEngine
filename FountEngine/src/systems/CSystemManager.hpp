#pragma once
#include <memory>

class CEntitySystem;
class CFileSystem;
class CInputSystem;
class CLogSystem;
class CResourceSystem;
class CPostProcessSystem;

extern CEntitySystem* g_pEntitySystem;
extern CFileSystem* g_pFileSystem;
extern CInputSystem* g_pInputSystem;
extern CLogSystem* g_pLogSystem;
extern CResourceSystem* g_pResourceSystem;
extern CPostProcessSystem* g_pPostProcessSystem;

class CSystemManager {
public:
	static CSystemManager& GetInstance();

	bool Initialize();
	void Shutdown();

private:
	CSystemManager() = default;

	std::unique_ptr<CEntitySystem> m_pEntitySystem;
	std::unique_ptr<CFileSystem> m_pFileSystem;
	std::unique_ptr<CInputSystem> m_pInputSystem;
	std::unique_ptr<CLogSystem> m_pLogSystem;
	std::unique_ptr<CResourceSystem> m_pResourceSystem;
	std::unique_ptr<CPostProcessSystem> m_pPostProcessSystem;
};