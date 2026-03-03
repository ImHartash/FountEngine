#pragma once

// TODO: Make simple filesystem for FountEngine

class CFileSystem {
public:
	CFileSystem& GetInstance();

	void Initialize();

private:
	CFileSystem();
	~CFileSystem() = default;
};