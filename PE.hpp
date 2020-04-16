//Copyright 2020 Iris Technologies, All Rights Reserved
#pragma once
#include <Console.hpp>
#include <Utilities.hpp>
#include <Demangler.hpp>

namespace ProjectIris
{
    namespace PE
    {
        constexpr unsigned long long ImageBase64 = 4194304;

        constexpr unsigned int ImageBase32 = 4194304;

        __forceinline unsigned int CalculateSectionVA(const unsigned int SizeOfLastSection, const unsigned int VAOfLastSection) noexcept
        {
            return VAOfLastSection + (SizeOfLastSection / 4096 + 1) * 4096;
        }

        __forceinline void WriteNumberOfSections(const unsigned short number) noexcept
        {
            Bytes2 b = Utilities::UshortToBytes(number);
            Database::Binary[254] = b.b;
            Database::Binary[255] = b.a;
        }

        __forceinline void WriteTimeStamp() noexcept
        {
            Bytes4 b = Utilities::UintToBytes(static_cast<unsigned int>(chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count()));
            Database::Binary[256] = b.d;
            Database::Binary[257] = b.c;
            Database::Binary[258] = b.b;
            Database::Binary[259] = b.a;
        }

        __forceinline void WriteSizeOfCode(const unsigned int size) noexcept
        {
            Bytes4 b = Utilities::UintToBytes(size);
            Database::Binary[276] = b.d;
            Database::Binary[277] = b.c;
            Database::Binary[278] = b.b;
            Database::Binary[279] = b.a;
        }

        __forceinline void WriteSizeOfRdataAndData(const unsigned int size) noexcept
        {
            Bytes4 b = Utilities::UintToBytes(size);
            Database::Binary[280] = b.d;
            Database::Binary[281] = b.c;
            Database::Binary[282] = b.b;
            Database::Binary[283] = b.a;
        }

        __forceinline void WriteSizeOfBss(const unsigned int size) noexcept
        {
            Bytes4 b = Utilities::UintToBytes(size);
            Database::Binary[284] = b.d;
            Database::Binary[285] = b.c;
            Database::Binary[286] = b.d;
            Database::Binary[287] = b.a;
        }

        __forceinline void WriteOSVersions() noexcept
        {
            Bytes2 b = Utilities::UshortToBytes(Database::MajorWindowsVersion);
            Database::Binary[312] = b.b;
            Database::Binary[313] = b.a;
            Database::Binary[320] = b.b;
            Database::Binary[321] = b.a;
            b = Utilities::UshortToBytes(Database::MinorWindowsVersion);
            Database::Binary[314] = b.b;
            Database::Binary[315] = b.a;
            Database::Binary[322] = b.b;
            Database::Binary[323] = b.a;
        }

        __forceinline void WriteAppVersions() noexcept
        {
            Bytes2 b = Utilities::UshortToBytes(Database::MajorAppVersion);
            Database::Binary[316] = b.b;
            Database::Binary[317] = b.a;
            b = Utilities::UshortToBytes(Database::MinorAppVersion);
            Database::Binary[318] = b.b;
            Database::Binary[319] = b.a;
        }

        __forceinline void WriteSizeOfApp() noexcept
        {
            const size_t s = Database::NativeSections.size() - 1;
            Bytes4 b = Utilities::UintToBytes(((Database::NativeSections[s].VirtualAddress + (Database::NativeSections[s].VirtualAddress + Database::NativeSections[s].VirtualSize) - Database::NativeSections[s].PointerToRawData) / 4096 + 1) * 4096);
            Database::Binary[328] = b.d;
            Database::Binary[329] = b.c;
            Database::Binary[330] = b.b;
            Database::Binary[331] = b.a;
        }

        __forceinline void WriteSubsystem() noexcept
        {
            if(Database::os == OperatingSystem::Xbox)
                Database::Binary[340] = 0xe;
            else if(Database::IsConsole)
                Database::Binary[340] = 3;
            else
                Database::Binary[340] = 2;
        }

        __forceinline void WriteSizeOfReservedStack(const unsigned long long size) noexcept
        {
            Bytes8 b = Utilities::UlongToBytes(size);
            Database::Binary[344] = b.h;
            Database::Binary[345] = b.g;
            Database::Binary[346] = b.f;
            Database::Binary[347] = b.e;
            Database::Binary[348] = b.d;
            Database::Binary[349] = b.c;
            Database::Binary[350] = b.b;
            Database::Binary[351] = b.a;
        }

        __forceinline void WriteSizeOfCommitStack(const unsigned long long size) noexcept
        {
            Bytes8 b = Utilities::UlongToBytes(size);
            Database::Binary[352] = b.h;
            Database::Binary[353] = b.g;
            Database::Binary[354] = b.f;
            Database::Binary[355] = b.e;
            Database::Binary[356] = b.d;
            Database::Binary[357] = b.c;
            Database::Binary[358] = b.b;
            Database::Binary[359] = b.a;
        }

        __forceinline void WriteSizeOfReservedHeap(const unsigned long long size) noexcept
        {
            Bytes8 b = Utilities::UlongToBytes(size);
            Database::Binary[360] = b.h;
            Database::Binary[361] = b.g;
            Database::Binary[362] = b.f;
            Database::Binary[363] = b.e;
            Database::Binary[364] = b.d;
            Database::Binary[365] = b.c;
            Database::Binary[366] = b.b;
            Database::Binary[367] = b.a;
        }

        __forceinline void WriteSizeOfCommitHeap(const unsigned long long size) noexcept
        {
            Bytes8 b = Utilities::UlongToBytes(size);
            Database::Binary[368] = b.h;
            Database::Binary[369] = b.g;
            Database::Binary[370] = b.f;
            Database::Binary[371] = b.e;
            Database::Binary[372] = b.d;
            Database::Binary[373] = b.c;
            Database::Binary[374] = b.b;
            Database::Binary[375] = b.a;
        }

        __forceinline unsigned int RoundToLineAlignment(const unsigned int size) noexcept
        {
            if(size % static_cast<unsigned int>(16) == 0)
                return size;
            else
                return (size / static_cast<unsigned int>(16) + static_cast<unsigned int>(1)) * static_cast<unsigned int>(16);
        }

        __forceinline unsigned int RoundToFileAlignment(const unsigned int size) noexcept
        {
            if(size % static_cast<unsigned int>(512) == 0)
                return size;
            else
                return (size / static_cast<unsigned int>(512) + static_cast<unsigned int>(1)) * static_cast<unsigned int>(512);
        }

        __forceinline unsigned int RoundFileToMemoryAlignment(const unsigned int size) noexcept
        {
            if(size % static_cast<unsigned int>(512) == 0)
                return size / 512 * 4096;
            else
                return (size / static_cast<unsigned int>(512) + static_cast<unsigned int>(1)) * static_cast<unsigned int>(4096);
        }

        __forceinline size_t RoundToMemoryAlignment(const size_t size) noexcept
        {
            return (size / static_cast<size_t>(4096) + static_cast<size_t>(1)) * static_cast<size_t>(4096);
        }

        __forceinline void FillToFitFileAlignment(const size_t size) noexcept
        {
            const size_t RTFA = RoundToFileAlignment(size);
            if(RTFA != size)
                Database::Binary.resize(Database::Binary.size() + (RTFA - size));
        }

        __forceinline void WriteTextSection() noexcept
        {
            PENativeSection text;
            text.PointerToRawData = static_cast<unsigned int>(1024);
            text.VirtualAddress = static_cast<unsigned int>(4096);
            text.SizeOfRawData = static_cast<unsigned int>(0);
            text.VirtualSize = static_cast<unsigned int>(0);
            Database::NativeSections.push_back(text);
        }

        __forceinline void WriteTextSectionSize(const unsigned int size, const unsigned int roundedsize) noexcept
        {
            Database::NativeSections[0].VirtualSize = size;
            Database::NativeSections[0].SizeOfRawData = roundedsize;
            Bytes4 b = Utilities::UintToBytes(roundedsize);
            Database::Binary[528] = b.d;
            Database::Binary[529] = b.c;
            Database::Binary[530] = b.b;
            Database::Binary[531] = b.a;
            b = Utilities::UintToBytes(size);
            Database::Binary[520] = b.d;
            Database::Binary[521] = b.c;
            Database::Binary[522] = b.b;
            Database::Binary[523] = b.a;
        }

        __forceinline unsigned int RVAToFileOffset(const vector<PESection>& Sections, const unsigned int RVA) noexcept
        {
            size_t SectionOffset = 0;
            for(; SectionOffset < Sections.size(); ++SectionOffset)
                if(RVA >= Sections[SectionOffset].VirtualAddress && RVA < Sections[SectionOffset].VirtualEnd)
                    break;
            return RVA - Sections[SectionOffset].VirtualAddress + Sections[SectionOffset].PointerToRawData;
        }

        __forceinline void ReadExportedSymbols() noexcept
        {
            for(size_t x = 0; x < Database::NativeLibraries.size(); ++x)
            {
                const chrono::steady_clock::time_point a = Utilities::TimePoint();
                const vector<unsigned char> Content = Utilities::ReadBinary(Database::NativeLibraries[x].Name);
                if(Content[0] == 0x4d && Content[1] == 0x5a)
                {
                        unsigned int Offset = Utilities::BytesToUint(Content[60], Content[61], Content[62], Content[63]);
                        if(Content[Offset] == 0x50 && Content[Offset + 1] == 0x45 && Content[Offset + 2] == 0 && Content[Offset + 3] == 0)
                        {
                            unsigned short NumberOfSections = Utilities::BytesToUshort(Content[Offset + 6], Content[Offset + 7]);
                            unsigned int ExportRVA;
                            if(Content[Offset + 25] == 2)
                            {
                                ExportRVA = Utilities::BytesToUint(Content[Offset + 136], Content[Offset + 137], Content[Offset + 138], Content[Offset + 139]);
                                Offset += 272;
                            }
                            else
                            {
                                ExportRVA = Utilities::BytesToUint(Content[Offset + 120], Content[Offset + 121], Content[Offset + 122], Content[Offset + 123]);
                                Offset += 256;
                            }
                            if(ExportRVA == 0)
                            {
                                Console::LinkerError("\n\nThe native library '" + Database::NativeLibraries[x].Name + "' does not contain any export table");
                                break;
                            }
                            else
                            {
                                vector<PESection> Sections(NumberOfSections);
                                for(unsigned short us = 0; us < NumberOfSections; ++us, Offset += 40)
                                {
                                    PESection pes;
                                    unsigned int VirtualSize = Utilities::BytesToUint(Content[Offset], Content[Offset + 1], Content[Offset + 2], Content[Offset + 3]);
                                    pes.VirtualAddress = Utilities::BytesToUint(Content[Offset + 4], Content[Offset + 5], Content[Offset + 6], Content[Offset + 7]);
                                    pes.VirtualEnd = pes.VirtualAddress + VirtualSize;
                                    pes.PointerToRawData = Utilities::BytesToUint(Content[Offset + 12], Content[Offset + 13], Content[Offset + 14], Content[Offset + 15]);
                                    Sections[us] = pes;
                                }
                                Offset = RVAToFileOffset(Sections, ExportRVA) + 20;
                                const unsigned int ESS = static_cast<unsigned int>(Database::ExportedSymbols.size());
                                unsigned int NumberOfFunctions = ESS + Utilities::BytesToUint(Content[Offset], Content[Offset + 1], Content[Offset + 2], Content[Offset + 3]);
                                Database::ExportedSymbols.resize(NumberOfFunctions);
                                Offset = RVAToFileOffset(Sections, Utilities::BytesToUint(Content[Offset + 12], Content[Offset + 13], Content[Offset + 14], Content[Offset + 15]));
                                for(unsigned int i = ESS; i < NumberOfFunctions; ++i, Offset += 4)
                                {
                                    string Name;
                                    unsigned int NameOffset = RVAToFileOffset(Sections, Utilities::BytesToUint(Content[Offset], Content[Offset + 1], Content[Offset + 2], Content[Offset + 3]));
                                    while(Content[NameOffset] != 0)
                                    {
                                        Name.push_back(static_cast<char>(Content[NameOffset]));
                                        ++NameOffset;
                                    }
                                    Database::ExportedSymbols[i] = Utilities::Hash(Demangler::Demangle(Name));
                                }
                            }
                        }
                        else
                        {
                            Console::LinkerError("\n\nThe native library '" + Database::NativeLibraries[x].Name + "' does not have any PE\\0\\0 signature");
                            break;
                        }
                    }
                    else
                    {
                        Console::LinkerError("\n\nThe native library '" + Database::NativeLibraries[x].Name + "' does not have any MZ signature");
                        break;
                    }
                    const chrono::steady_clock::time_point b = Utilities::TimePoint();
                    Console::Append("\n\nReading and extracting the exported symbols of the native library '" + Database::NativeLibraries[x].Name + "' took " + Utilities::TimeDiff(a, b));
                }
        }
    }
}