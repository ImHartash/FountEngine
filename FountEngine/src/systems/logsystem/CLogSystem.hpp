#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <vector>
#include <windows.h>

class CLogSystem {
public:
	enum ELogLevel {
		LEVEL_DEBUG,
		LEVEL_INFO,
		LEVEL_WARNING,
		LEVEL_ERROR,
		LEVEL_FATAL
	};

	enum ELogColor {
		COLOR_DEFAULT = 7,
		COLOR_DEBUG = 8,
		COLOR_INFO = 10,
		COLOR_WARNING = 14,
		COLOR_ERROR = 12,
		COLOR_FATAL = 13,
		COLOR_TIMESTAMP = 11
	};

	CLogSystem();
	~CLogSystem();

	bool Initialize(const std::string& strLogFile = "engine.log");
	void Shutdown();

	void Log(ELogLevel Level, const std::string& strMessage);
	void LogF(ELogLevel Level, const char* szFormat, ...);
	void LogF(ELogLevel Level, const char* szFile, int nLine, const char* szFormat, ...);

	void SetConsoleColorsEnabled(bool bEnabled) { m_bConsoleColors = bEnabled; }
	void SetMinLogLevel(ELogLevel Level) { m_eMinLogLevel = Level; }

private:
	std::string GetCurrentTimeStamp();
	std::string LevelToString(ELogLevel Level);
	ELogColor LevelToColor(ELogLevel Level);
	void SetConsoleColor(ELogColor Color);
	void WriteToFile(const std::string& strMessage);
	void WriteToConsole(const std::string& strMessage, ELogLevel Level);

	bool m_bInitialized = false;
	bool m_bConsoleColors = true;
	ELogLevel m_eMinLogLevel = LEVEL_DEBUG;

	std::ofstream m_LogFile;
	std::mutex m_Mutex;
	HANDLE m_hConsole;
};

#define LOG_DEBUG(...)    g_pLogSystem->LogF(CLogSystem::LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)     g_pLogSystem->LogF(CLogSystem::LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...)  g_pLogSystem->LogF(CLogSystem::LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)    g_pLogSystem->LogF(CLogSystem::LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)    g_pLogSystem->LogF(CLogSystem::LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#define LOG_DEBUG_S(...)    g_pLogSystem->LogF(CLogSystem::LEVEL_DEBUG, __VA_ARGS__)
#define LOG_INFO_S(...)     g_pLogSystem->LogF(CLogSystem::LEVEL_INFO, __VA_ARGS__)