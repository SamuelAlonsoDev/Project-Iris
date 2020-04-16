//Copyright 2020 Iris Technologies, All Rights Reserved
#pragma once
#include <PE.hpp>

namespace ProjectIris
{
    namespace Linker
    {
        __forceinline void Process() noexcept
        {
            if(Database::os == OperatingSystem::Windows)
                PE::ReadExportedSymbols();
        }

        __forceinline void LinkPE() noexcept
        {
        	const chrono::steady_clock::time_point a = Utilities::TimePoint();
			unsigned short NumberOfSections = static_cast<unsigned short>(1);
			PE::FillToFitFileAlignment(static_cast<unsigned int>(40));
			Database::Binary.insert(Database::Binary.end(), Database::Text.begin(), Database::Text.end());
			unsigned int SizeOfCode = static_cast<unsigned int>(Database::Text.size());
			SizeOfCode = PE::RoundToLineAlignment(SizeOfCode);
			PE::FillToFitFileAlignment(SizeOfCode);
			unsigned int VSizeOfCode = PE::RoundToFileAlignment(SizeOfCode);
			PE::WriteTextSectionSize(SizeOfCode, VSizeOfCode);
			PE::WriteNumberOfSections(NumberOfSections);
			PE::WriteSizeOfCode(VSizeOfCode);
			PE::WriteOSVersions();
			PE::WriteAppVersions();
			PE::WriteTimeStamp();
			PE::WriteSubsystem();
			PE::WriteSizeOfApp();
			const chrono::steady_clock::time_point b = Utilities::TimePoint();
			Console::Append("\n\nLinking the executable took " + Utilities::TimeDiff(a, b));
        }
    }
}