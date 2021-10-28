#pragma once
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <fileapi.h>
#include <vector>
//#include <Shlwapi.h>

#include <random>

using namespace std;
class Workspace
{
protected: // Attributes
	Workspace* parent_;
	wstring tempPath_;
	wstring dirName_;
private:
	static const size_t randomStrLen = 6;
private: // Utility
	static string randomString(size_t length)
	{
		std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

		std::random_device rd;
		std::mt19937 generator(rd());

		std::shuffle(str.begin(), str.end(), generator);

		return str.substr(0, length);
	}

	static wstring createDirName(wstring prefix = L"tmp_")
	{
		const string strRand = randomString(randomStrLen);
		wstring wsRand(strRand.begin(), strRand.end());
		return  prefix + wsRand;
	}

	static wstring trimEndChar(wstring dir, const wchar_t trimChar)
	{
		if (dir.back() == trimChar)
		{
			const wchar_t trimChars[] = { trimChar };
			dir.erase(dir.find_last_not_of(trimChars) + 1U);
		}

		return dir;
	}

	static DWORD deleteAllFiles(wstring dir)
	{
		WIN32_FIND_DATA ffd;
		LARGE_INTEGER filesize;
		
		wstring wildDir = dir + L"*";

		HANDLE hFind = FindFirstFile(wildDir.c_str(),&ffd);

		if (INVALID_HANDLE_VALUE == hFind)
		{
			cout << "error: find first file.\n";
			throw;
		}

		std::vector<wstring> dirs;

		do
		{
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// skip "." or ".."
				if (ffd.cFileName[0] == '.' && ffd.cFileName[1] == '\0') continue;
				if (ffd.cFileName[0] == '.' && ffd.cFileName[1] == '.' && ffd.cFileName[2] == '\0') continue;

				wcout << ffd.cFileName << L"  <DIR>" << endl;

				// サブディレクトリのパスを保存しておく。
				dirs.push_back(dir + ffd.cFileName + L"\\");
			}
			else
			{
				filesize.LowPart = ffd.nFileSizeLow;
				filesize.HighPart = ffd.nFileSizeHigh;
				wstring path =  dir + ffd.cFileName;
				wcout << ffd.cFileName << L"  <FILE>" << endl;
				DeleteFile(path.c_str());							
			}
		}    
		while (FindNextFile(hFind, &ffd) != FALSE);

		DWORD dwError = GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
		{
			cout << "error.\n";
			throw;
		}

		FindClose(hFind);

		// サブディレクトリを削除する。
		const size_t size = dirs.size();
		for (size_t i = 0; i < size; i++)
		{
			deleteAllFiles(dirs[i]);
			RemoveDirectory(dirs[i].c_str());
		}

		return dwError;
	}

public:
	// Constructor/Destructor
	Workspace(wstring prefix = L"temp_") : parent_(nullptr)
	{
		// top most workspace
		const DWORD len = MAX_PATH;
		TCHAR buf[len] = {NULL};
		GetTempPath(len, buf);
		tempPath_ = buf;
		wcout << tempPath_.c_str() << endl;

		dirName_ = createDirName(prefix);
		wcout << dirName_.c_str() << endl;

		CreateDirectory(GetPath().c_str(),NULL);
	}

	Workspace(Workspace* parent, wstring prefix = L"temp_") :parent_(parent)
	{
		dirName_ = createDirName(prefix);
		wcout << dirName_.c_str() << endl;

		CreateDirectory(GetPath().c_str(), NULL);
	}

	~Workspace()
	{
		deleteAllFiles(GetPath());
		RemoveDirectory(trimEndChar(GetPath(),L'\\').c_str());
	}

	Workspace(const Workspace& obj) = delete;
public:
	// Implements
	virtual wstring GetPath()
	{
		if (parent_ == nullptr)
			return tempPath_ + dirName_ + L"\\";
		else
			return parent_->GetPath() + dirName_ + L"\\";
	}

};

