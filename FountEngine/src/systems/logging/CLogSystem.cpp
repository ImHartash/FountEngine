#include "CLogSystem.hpp"
#include <iostream>
#include <ctime>
#include <cstdarg>
#include <sstream>

#ifdef _DEBUG
#	include <windows.h>
#endif

CLogSystem::CLogSystem() : m_hConsole(GetStdHandle(STD_OUTPUT_HANDLE)) {}
CLogSystem::~CLogSystem() {
	Shutdown();
}

CLogSystem& CLogSystem::GetInstance() {
	static CLogSystem Instance;
	return Instance;
}

bool CLogSystem::Initialize(const std::string& strLogFile) {
	std::lock_guard<std::mutex> lock(m_Mutex);

	if (m_bInitialized) {
		return true;
	}

	m_LogFile.open(strLogFile, std::ios::out | std::ios::trunc);
	if (!m_LogFile.is_open()) {
		return false;
	}

#ifdef _DEBUG
	if (m_bConsoleColors) {
		if (AllocConsole()) {
			FILE* fDummy;
			freopen_s(&fDummy, "CONOUT$", "w", stdout);
			freopen_s(&fDummy, "CONOUT$", "w", stderr);
			m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

			SetConsoleTitleA("Debug Console - Fount Engine");
		}
	}
#endif

	m_bInitialized = true;
	return true;
}

void CLogSystem::Shutdown() {
	std::lock_guard <std::mutex> lock(m_Mutex);

	if (m_bInitialized) {
		if (m_LogFile.is_open()) {
			m_LogFile.flush();
			m_LogFile.close();
		}

		m_bInitialized = false;
	}
}

void CLogSystem::Log(ELogLevel Level, const std::string& strMessage) {
	if (Level < m_eMinLogLevel || !m_bInitialized) {
		return;
	}

	std::lock_guard<std::mutex> lock(m_Mutex);

	std::stringstream FormattedMessage;
	std::string strTimestamp = GetCurrentTimeStamp();
	std::string strLevel = LevelToString(Level);

	FormattedMessage << "[" << strTimestamp << "] [" << strLevel << "] " << strMessage;

	std::string strFinalMessage = FormattedMessage.str();

	WriteToFile(strFinalMessage);
	WriteToConsole(strFinalMessage, Level);
}

void CLogSystem::LogF(ELogLevel Level, const char* szFormat, ...) {
	if (Level < m_eMinLogLevel || !m_bInitialized) {
		return;
	}

	char cBuffer[1024];
	va_list args;
	va_start(args, szFormat);
	vsprintf_s(cBuffer, szFormat, args);
	va_end(args);

	Log(Level, cBuffer);
}

void CLogSystem::LogF(ELogLevel Level, const char* szFile, int nLine, const char* szFormat, ...) {
	if (Level < m_eMinLogLevel || !m_bInitialized) {
		return;
	}

	char cMessageBuffer[1024];
	va_list args;
	va_start(args, szFormat);
	vsprintf_s(cMessageBuffer, szFormat, args);
	va_end(args);

	char cFinalBuffer[2048];
	sprintf_s(cFinalBuffer, "%s (%s:%d)", cMessageBuffer, szFile, nLine);

	Log(Level, cFinalBuffer);
}

std::string CLogSystem::GetCurrentTimeStamp() {
	time_t now = time(nullptr);
	tm timeInfo;
	localtime_s(&timeInfo, &now);

	char cBuffer[80];
	strftime(cBuffer, sizeof(cBuffer), "%H:%M:%S", &timeInfo);

	return std::string(cBuffer);
}

std::string CLogSystem::LevelToString(ELogLevel Level) {
	switch (Level) {
	case LEVEL_DEBUG: return "DEBUG";
	case LEVEL_INFO: return "INFO";
	case LEVEL_WARNING: return "WARNING";
	case LEVEL_ERROR: return "ERROR";
	case LEVEL_FATAL: return "FATAL";
	default: return "UNKNOWN";
	}
}

CLogSystem::ELogColor CLogSystem::LevelToColor(ELogLevel Level) {
	switch (Level) {
	case LEVEL_DEBUG: return COLOR_DEBUG;
	case LEVEL_INFO: return COLOR_INFO;
	case LEVEL_WARNING: return COLOR_WARNING;
	case LEVEL_ERROR: return COLOR_ERROR;
	case LEVEL_FATAL: return COLOR_FATAL;
	default: return COLOR_DEFAULT;
	}
}

void CLogSystem::SetConsoleColor(ELogColor Color) {
	if (m_bConsoleColors && m_hConsole) {
		SetConsoleTextAttribute(m_hConsole, Color);
	}
}

void CLogSystem::WriteToFile(const std::string& strMessage) {
	if (m_LogFile.is_open()) {
		m_LogFile << strMessage << std::endl;
	}
}

void CLogSystem::WriteToConsole(const std::string& strMessage, ELogLevel Level) {
	if (!m_bConsoleColors || !m_hConsole) {
		std::cout << strMessage << std::endl;
		return;
	}

	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(m_hConsole, &consoleInfo);
	WORD wOriginalColor = consoleInfo.wAttributes;

	SetConsoleColor(LevelToColor(Level));
	std::cout << strMessage << std::endl;

	SetConsoleTextAttribute(m_hConsole, wOriginalColor);
}