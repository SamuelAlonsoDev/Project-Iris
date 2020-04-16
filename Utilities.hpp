//Copyright 2020 Iris Technologies, All Rights Reserved
#pragma once
#include <Iris.hpp>
#include <Database.hpp>
#include <Structures.hpp>
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <algorithm>
#include <sys/stat.h>
#include <filesystem>

using namespace std;

namespace ProjectIris
{
	namespace Utilities
	{
		__forceinline vector<string> GetFilesInFolder(const string& Path, const bool IsRetina = false) noexcept
		{
			vector<string> v;
			for (const auto& File : filesystem::recursive_directory_iterator(Path))
			{
				string f = File.path().string();
				if(f.size() > static_cast<size_t>(6))
				{
					if(!IsRetina)
					{
						if (f[f.size() - 1] == 's' && f[f.size() - 2] == 'i' && f[f.size() - 3] == 'r' && f[f.size() - 4] == 'i' && f[f.size() - 5] == '.')
						{
							bool Exists = false;
							for(size_t x = 0; x < Database::IgnoredIris.size(); ++x)
								if(Database::IgnoredIris[x] == f)
								{
									Exists = true;
									Database::ConsoleBuffer.append("\n\nIgnored iris file '" + f + "'");
									break;
								}
							if(!Exists)
								v.push_back(f);
						}
					}
					else if(f[f.size() - 1] == 'a' && f[f.size() - 2] == 'n' && f[f.size() - 3] == 'i' && f[f.size() - 4] == 't' && f[f.size() - 5] == 'e' && f[f.size() - 6] == 'r' && f[f.size() - 7] == '.')
					{
						bool Exists = false;
						for(size_t x = 0; x < Database::IgnoredRetina.size(); ++x)
							if(Database::IgnoredRetina[x] == f)
							{
								Exists = true;
								Database::ConsoleBuffer.append("\n\nIgnored retina file '" + f + "'");
								break;
							}
						if(!Exists)
							v.push_back(f);
					}
				}
			}
			return v;
		}

		__forceinline void InitApp(const string& path) noexcept
		{
			STARTUPINFOA info={sizeof(info)};
			PROCESS_INFORMATION processInfo;
			if (CreateProcessA(path.c_str(), Database::Commands.data(), 0, 0, 1, 0, 0, 0, &info, &processInfo))
			{
    			WaitForSingleObject(processInfo.hProcess, INFINITE);
			    CloseHandle(processInfo.hProcess);
    			CloseHandle(processInfo.hThread);
			}
		}

		__forceinline size_t Hash(const string& s) noexcept
		{
			return hash<string>()(s);
		}

		__forceinline bool FileExists(const string& Path) noexcept
		{
			DWORD dwAttrib = GetFileAttributesA(Path.c_str());
  			return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
		}

		__forceinline bool DirectoryExists(const string& Path) noexcept
		{
			DWORD dwAttrib = GetFileAttributesA(Path.c_str());
			return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
		}

		__forceinline string ReadFile(const string& Path) noexcept
		{
			string s;
			OVERLAPPED o = {0};
			LARGE_INTEGER size;
			HANDLE h = CreateFileA(Path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 0);
			if(h != INVALID_HANDLE_VALUE)
			{
				GetFileSizeEx(h, &size);
				s.resize(size.LowPart);
				ReadFileEx(h, &s[0], size.LowPart, &o, 0);
				CloseHandle(h);
			}
			s.erase(remove(s.begin(), s.end(), '\t'), s.end());
			s.erase(remove(s.begin(), s.end(), '\r'), s.end());
			return s;
		}

		__forceinline void WriteFile(const string& Path, const string& Content) noexcept
		{
			if(FileExists(Path))
				DeleteFileA(Path.c_str());
			HANDLE h = CreateFileA(Path.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
			if(h != INVALID_HANDLE_VALUE)
			{
				OVERLAPPED o = {0};
				WriteFileEx(h, &Content[0], Content.size(), &o, 0);
				CloseHandle(h);
			}
		}

		__forceinline void CreateDirectory(const string& Path) noexcept
		{
			filesystem::create_directories(Path);
		}

		__forceinline vector<unsigned char> ReadBinary(const string& Path) noexcept
		{
			vector<unsigned char> s;
			OVERLAPPED o = {0};
			LARGE_INTEGER size;
			HANDLE h = CreateFileA(Path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 0);
			if(h != INVALID_HANDLE_VALUE)
			{
				GetFileSizeEx(h, &size);
				s.resize(size.LowPart);
				ReadFileEx(h, &s[0], size.LowPart, &o, 0);
				CloseHandle(h);
			}
			return s;
		}

		__forceinline void WriteBinary(const string& Path, const vector<unsigned char>& Content) noexcept
		{
			if(FileExists(Path))
				DeleteFileA(Path.c_str());
			HANDLE h = CreateFileA(Path.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
			if(h != INVALID_HANDLE_VALUE)
			{
				OVERLAPPED o = {0};
				WriteFileEx(h, &Content[0], Content.size(), &o, 0);
				CloseHandle(h);
			}
		}

		__forceinline string GetDirectory(const string& Path) noexcept
		{
			return Path.substr(0, Path.find_last_of("\\/"));
		}

		__forceinline bool IsOnlyNumeric(const string& s) noexcept
		{
			for(size_t x = 0; x < s.size(); ++x)
				if(s[x] < '0' || s[x] > '9')
					return false;
			return true;
		}

		__forceinline bool nIsOnlyNumeric(const char* s, const size_t s_size) noexcept
		{
			for(size_t x = 0; x < s_size; ++x)
				if(s[x] < '0' || s[x] > '9')
					return false;
			return true;
		}

		__forceinline bool IsAlphanumeric(const char c) noexcept
		{
			return (c == '_' || (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
		}

		__forceinline string to_stringc(size_t Character) noexcept
		{
			if(Character == 0)
				Character = 1;
			return to_string(Character);
		}

		__forceinline void Report(const string& File, const size_t Line, size_t Character, const string& Message) noexcept
		{
			if(Character == 0)
				Character = 1;
			Database::PupilError = true;
			if(Database::UseVisualDebugger)
			{
				Database::Report.File = File;
				Database::Report.Line = Line;
				Database::Report.Character = Character;
				Database::Report.Message = Message;
			}
			else
			{
				const string m = "\n\nError at the file '" + File + "', " + to_string(Line) + " : " + to_string(Character) + " -> " + Message;
				Database::ConsoleBuffer.append(m);
			}
		}

		__forceinline void Cleanup() noexcept
		{
			if(Database::os == OperatingSystem::Windows || Database::os == OperatingSystem::Xbox)
				Database::Binary.resize(552);
			Database::InitApp = false;
			Database::LastMessage = false;
			Database::IsConsole = false;
			Database::LinkerError = false;
			Database::PupilError = false;
			Database::UseVisualDebugger = false;
			Database::ReportPupil = false;
			Database::mtp = MeasureTypes::Microseconds;
			Database::os = OperatingSystem::Windows;
			Database::pt = ProjectType::Sandbox;
			Database::MajorWindowsVersion = 5;
			Database::MinorWindowsVersion = 2;
			Database::MajorAppVersion = 0;
			Database::MinorAppVersion = 0;
			Database::ActualLine = 0;
			Database::ActualCharacter = 0;
			Database::Report.clear();
			Database::ActualFile.clear();
			Database::PupilFiles.clear();
			Database::IgnoredIris.clear();
			Database::ConsoleBuffer.clear();
			Database::ExportedSymbols.clear();
			Database::IrisFiles.clear();
			Database::NativeSections.clear();
			Database::NativeLibraries.clear();
			Database::ProjectName.clear();
			Database::PupilFInstructions.clear();
			Database::Text.clear();
			Database::Commands.clear();
		}

		__forceinline void Recompile() noexcept
		{
			const string h(Database::ConsoleBuffer + "\n\n\nPress 'r' to execute again or any other key to exit...");
			cout << h << endl;
			if(Database::InitApp)
				InitApp(Database::OutputFolder + Database::ProjectName + ".exe");
			char c;
			cin >> c;
			if (c == 'r')
			{
				Cleanup();	
				Execute();
			}
		}

		__forceinline chrono::steady_clock::time_point TimePoint() noexcept
		{
			return chrono::steady_clock::now();
		}

		__forceinline string TimeDiff(const chrono::steady_clock::time_point& begin, const chrono::steady_clock::time_point& end) noexcept
		{
			if(Database::mtp == MeasureTypes::Nanoseconds)
				return to_string(chrono::duration_cast<chrono::nanoseconds>(end - begin).count()) + " nanoseconds";
			else if(Database::mtp == MeasureTypes::Microseconds)
				return to_string(chrono::duration_cast<chrono::microseconds>(end - begin).count()) + " microseconds";
			return to_string(chrono::duration_cast<chrono::milliseconds>(end - begin).count()) + " milliseconds";
		}

		__forceinline double TimeDiffD(const chrono::steady_clock::time_point& begin, const chrono::steady_clock::time_point& end) noexcept
		{
			return chrono::duration_cast<chrono::microseconds>(end - begin).count();
		}

		__forceinline unsigned int BytesToUint(const unsigned char a, const unsigned char b, const unsigned char c, const unsigned char d) noexcept
		{
			return ((d << 24) | (c << 16) | (b << 8) | a);
		}

		__forceinline Bytes4 UintToBytes(const unsigned int u) noexcept
		{
			return {static_cast<unsigned char>((u & 0xff000000) >> 24), static_cast<unsigned char>((u & 0x00ff0000) >> 16), static_cast<unsigned char>((u & 0x0000ff00) >> 8), static_cast<unsigned char>(u & 0x000000ff)};
		}

		__forceinline unsigned short BytesToUshort(const unsigned char a, const unsigned char b) noexcept
		{
			return ((b << 8) | a);
		}

		__forceinline Bytes2 UshortToBytes(const unsigned short u) noexcept
		{
			return {static_cast<unsigned char>((u & 0xff00) >> 8), static_cast<unsigned char>(u & 0x00ff)};
		}

		__forceinline Bytes8 UlongToBytes(const unsigned long long u) noexcept
		{
			return {static_cast<unsigned char>((u & 0xff00000000000000) >> 56), static_cast<unsigned char>((u &0x00ff000000000000) >> 48), static_cast<unsigned char>((u & 0x0000ff0000000000) >> 40), static_cast<unsigned char>((u & 0x000000ff00000000) >> 32), static_cast<unsigned char>((u & 0x00000000ff000000) >> 24), static_cast<unsigned char>((u & 0x0000000000ff0000) >> 16), static_cast<unsigned char>((u & 0x000000000000ff00) >> 8), static_cast<unsigned char>(u & 0x00000000000000ff)};
		}
	}
}