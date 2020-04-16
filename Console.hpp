//Copyright 2020 Iris Technologies, All Rights Reserved
#pragma once
#include <Database.hpp>
#include <string>
#include <iostream>

namespace ProjectIris
{
	namespace Console
	{
		__forceinline void Append(const string& Message) noexcept
		{
			Database::ConsoleBuffer.append(Message);
		}

		__forceinline void LinkerError(const string& Message) noexcept
		{
			Database::LinkerError = true;
			Database::ConsoleBuffer.append(Message);
		}

		__forceinline void PupilError(const string& File, const size_t Line, const size_t Character, const string& Message)
		{
			Database::ConsoleBuffer.append("\n\nError at '" + File + "', " + to_string(Line) + " : " + to_string(Character) + " -> " + Message);
		}
	}
}