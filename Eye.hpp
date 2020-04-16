//Copyright 2020 Iris Technologies, All Rights Reserved
#pragma once
#include <Pupil.hpp>
#include <Nerve.hpp>
#include <VisualDebugger.hpp>

namespace ProjectIris
{
    namespace Eye
    {
        __forceinline void Process() noexcept
        {
            PE::WriteTextSection();
            ErrorPoint ep;
            NativeLibrary lf;
            string Keyword;
            size_t Line = 1, Character = 0, LastLine, LastCharacter;
            bool IsIgnore = false, IsError = false, InvalidCharName = false, IsType = false, IsName = false, IsMeasureType = false, IsNatlib = false, FirstChar = false;
            chrono::steady_clock::time_point a = Utilities::TimePoint();
            const string EyeContent = Utilities::ReadFile(Database::ProjectPath);
            for(size_t x = 0; x < EyeContent.size(); ++x)
            {
                if(EyeContent[x] == '\n')
                {
                    ++Line;
                    Character = 0;
                }
                else
                    ++Character;
                if(IsNatlib)
                {
                    if(EyeContent[x] == '\n' || EyeContent[x] == ' ')
                    {
                        if (Keyword.size() > 0)
                        {
                            IsNatlib = false;
                            FirstChar = false;
                            if(Utilities::FileExists(Keyword))
                            {
                                bool Exists = false;
                                size_t ExistingLine, ExistingCharacter;
                                for(size_t xx = 0; xx < Database::NativeLibraries.size(); ++xx)
                                    if(Keyword == Database::NativeLibraries[xx].Name)
                                    {
                                        ExistingLine = Database::NativeLibraries[xx].Line;
                                        ExistingCharacter = Database::NativeLibraries[xx].Character;
                                        Exists = true;
                                        break;
                                    }
                                if(!Exists)
                                {
                                    lf.Line = LastLine;
                                    lf.Character = LastCharacter;
                                    lf.Name = Keyword;
                                    Database::NativeLibraries.push_back(lf);
                                }
                                else
                                {
                                    IsError = true;
                                    ep.Line = Line;
                                    ep.Character = Character;
                                    ep.Message = "The native library declaration '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + " is already specified at " + to_string(ExistingLine) + " : " + to_string(ExistingCharacter);
                                    break;
                                }
                            }
                            else
                            {
                                IsNatlib = false;
                                IsError = true;
                                ep.Line = Line;
                                ep.Character = Character;
                                ep.Message = "The native library declaration '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + ", does not exists";
                                Keyword.clear();
                                break;
                            }
                            Keyword.clear();
                        }
                    }
                    else if(!FirstChar)
                    {
                        FirstChar = true;
                        LastCharacter = Character;
                        LastLine = Line;
                        Keyword.push_back(EyeContent[x]);
                    }
                    else
                        Keyword.push_back(EyeContent[x]);
                }
                else if(IsIgnore)
                {
                    if(EyeContent[x] == '\n' || EyeContent[x] == ' ')
                    {
                        IsIgnore = false;
                        FirstChar = false;
                        if(Keyword.size() > 0)
                        {
                            if(!Utilities::FileExists(Keyword))
                            {
                                IsError = true;
                                ep.Line = Line;
                                ep.Character = Character;
                                ep.Message = "The ignored iris file declaration '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + " does not exists";
                                break;
                            }
                            else
                                Database::IgnoredIris.push_back(Keyword);
                            Keyword.clear();
                        }
                    }
                    else if(!FirstChar)
                    {
                        FirstChar = true;
                        LastLine = Line;
                        LastCharacter = Character;
                        Keyword.push_back(EyeContent[x]);
                    }
                    else
                        Keyword.push_back(EyeContent[x]);
                }
                else if(IsType)
                {
                    if(EyeContent[x] == '\n' || EyeContent[x] == ' ')
                    {
                        IsType = false;
                        FirstChar = false;
                        const size_t h = Utilities::Hash(Keyword);
                        if(h == Hashes::Executable)
                            Database::pt = ProjectType::Executable;
                        else if(h == Hashes::Library)
                            Database::pt = ProjectType::Library;
                        else if(h != Hashes::Sandbox)
                        {
                            IsError = true;
                            ep.Line = Line;
                            ep.Character = Character;
                            ep.Message = "The project type specifier '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + ", is unrecognized";
                            Keyword.clear();
                            break;
                        }
                        Keyword.clear();
                    }
                    else if(!FirstChar)
                    {
                        FirstChar = true;
                        LastLine = Line;
                        LastCharacter = Character;
                        Keyword.push_back(EyeContent[x]);
                    }
                    else
                        Keyword.push_back(EyeContent[x]);
                }
                else if(IsName)
                {
                    if(EyeContent[x] == '\n' || EyeContent[x] == ' ')
                    {
                        IsName = false;
                        FirstChar = false;
                        if(Utilities::IsOnlyNumeric(Keyword))
                        {
                            IsError = true;
                            ep.Line = Line;
                            ep.Character = Character;
                            ep.Message = "The project name '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + ", is only numeric";
                            Keyword.clear();
                            break;
                        }
                        else
                            Database::ProjectName = Keyword;
                        Keyword.clear();
                    }
                    else if(!Utilities::IsAlphanumeric(EyeContent[x]))
                    {
                        InvalidCharName = true;
                        IsError = true;
                        IsName = false;
                        ep.Line = Line;
                        ep.Character = Character;
                        const string str(1, EyeContent[x]);
                        if(!FirstChar)
                            ep.Message = "The first character '" + str + "' of the project name is not alphanumeric. Valid characters on the actual context are '_', from '0' to '9', from 'A' to 'Z', and from 'a' to 'z'"; 
                        else
                            ep.Message = "The character '" + str + "' of the project name starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + ", is not alphanumeric. Valid characters on the actual context are '_', from '0' to '9', from 'A' to 'Z', and from 'a' to 'z'";
                        Keyword.clear();
                        break;
                    }
                    else if(!FirstChar)
                    {
                        FirstChar = true;
                        LastLine = Line;
                        LastCharacter = Character;
                        Keyword.push_back(EyeContent[x]);
                    }
                    else
                        Keyword.push_back(EyeContent[x]);
                }
                else if(IsMeasureType)
                {
                    if(EyeContent[x] == '\n' || EyeContent[x] == ' ')
                    {
                        FirstChar = false;
                        IsMeasureType = false;
                        const size_t h = Utilities::Hash(Keyword);
                        if(h == Hashes::Nanoseconds)
                            Database::mtp = MeasureTypes::Nanoseconds;
                        else if(h == Hashes::Milliseconds)
                            Database::mtp = MeasureTypes::Milliseconds;
                        else if(h != Hashes::Microseconds)
                        {
                            IsMeasureType = false;
                            IsError = true;
                            ep.Line = Line;
                            ep.Character = Character;
                            ep.Message = "Wrong time-measurement type '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + ". Accepted commands are ms(milliseconds), mcs(microseconds), and ns(nanoseconds)";
                            Keyword.clear();
                            break;
                        }
                        Keyword.clear();
                    }
                    else if(!FirstChar)
                    {
                        FirstChar = true;
                        LastLine = Line;
                        LastCharacter = Character;
                        Keyword.push_back(EyeContent[x]);
                    }
                    else
                        Keyword.push_back(EyeContent[x]);
                }
                else if(EyeContent[x] == '\n' || EyeContent[x] == ' ')
                {
                    if(Keyword.size() > 0)
                    {
                        FirstChar = false;
                        const size_t h = Utilities::Hash(Keyword);
                        if(h == Hashes::Natlib)
                            IsNatlib = true;
                        else if(h == Hashes::IsConsole)
                            Database::IsConsole = true;
                        else if(h == Hashes::InitApp)
                            Database::InitApp = true;
                        else if(h == Hashes::Ignore)
                            IsIgnore = true;
                        else if(h == Hashes::UseVisualDebugger)
                            Database::UseVisualDebugger = true;
                        else if(h == Hashes::MeasureType)
                            IsMeasureType = true;
                        else if(h == Hashes::ReportPupil)
                            Database::ReportPupil = true;
                        else if(h == Hashes::Name)
                            IsName = true;
                        else if(h == Hashes::Type)
                            IsType = true;
                        else
                        {
                            IsError = true;
                            ep.Line = Line;
                            ep.Character = Character;
                            ep.Message = "The keyword '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + " is unrecognized";
                            break;
                        }
                        Keyword.clear();
                    }
                }
                else if(!FirstChar)
                {
                    FirstChar = true;
                    LastLine = Line;
                    LastCharacter = Character;
                    Keyword.push_back(EyeContent[x]);
                }
                else
                    Keyword.push_back(EyeContent[x]);
            }
            if(Keyword.size() > 0)
            {
                if(IsIgnore)
                {
                    if(!Utilities::FileExists(Keyword))
                    {
                        IsError = true;
                        ep.Line = Line;
                        ep.Character = Character;
                        ep.Message = "The ignored iris file '" + Keyword + "' does not exists";
                    }
                    else
                        Database::IgnoredIris.push_back(Keyword);
                }
                else if(IsNatlib)
                {
                    if(Utilities::FileExists(Keyword))
                    {
                        bool Exists = false;
                        size_t ExistingLine, ExistingCharacter;
                        for(size_t xx = 0; xx < Database::NativeLibraries.size(); ++xx)
                            if(Keyword == Database::NativeLibraries[xx].Name)
                            {
                                ExistingLine = Database::NativeLibraries[xx].Line;
                                ExistingCharacter = Database::NativeLibraries[xx].Character;
                                Exists = true;
                                break;
                            }
                        if(!Exists)
                        {
                            lf.Line = LastLine;
                            lf.Character = LastCharacter;
                            lf.Name = Keyword;
                            Database::NativeLibraries.push_back(lf);
                        }
                        else
                        {
                            IsError = true;
                            ep.Line = Line;
                            ep.Character = Character;
                            ep.Message = "The native library declaration '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + " is already specified at " + to_string(ExistingLine) + " : " + to_string(ExistingCharacter);
                        }
                    }
                    else
                    {
                        IsError = true;
                        ep.Line = Line;
                        ep.Character = Character;
                        ep.Message = "The native library declaration '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + ", does not exists";
                    }
                }
                else if(IsType)
                {
                    const size_t h = Utilities::Hash(Keyword);
                    if(h == Hashes::Executable)
                        Database::pt = ProjectType::Executable;
                    else if(h == Hashes::Library)
                        Database::pt = ProjectType::Library;
                    else if(h != Hashes::Sandbox)
                    {
                        IsError = true;
                        ep.Line = Line;
                        ep.Character = Character;
                        ep.Message = "The project type '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + ", is unrecognized";
                    }
                }
                else if(IsMeasureType)
                {
                    const size_t h = Utilities::Hash(Keyword);
                    if(h == Hashes::Nanoseconds)
                        Database::mtp = MeasureTypes::Nanoseconds;
                    else if(h == Hashes::Milliseconds)
                        Database::mtp = MeasureTypes::Milliseconds;
                    else if(h != Hashes::Microseconds)
                    {
                        IsError = true;
                        ep.Line = Line;
                        ep.Character = Character;
                        ep.Message = "Wrong time-measurement type '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + ". Accepted commands are ms(milliseconds), mcs(microseconds), and ns(nanoseconds)";
                    }
                }
                else if(IsName)
                {
                    if(Utilities::IsOnlyNumeric(Keyword))
                    {
                        IsError = true;
                        ep.Line = Line;
                        ep.Character = Character;
                        ep.Message = "The project name '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + ", is only numeric";
                    }
                    else
                        Database::ProjectName = Keyword;
                }
                else
                {
                    const size_t h = Utilities::Hash(Keyword);
                    if(h == Hashes::UseVisualDebugger)
                        Database::UseVisualDebugger = true;
                    else if(h == Hashes::ReportPupil)
                        Database::ReportPupil = true;
                    else if(h == Hashes::IsConsole)
                        Database::IsConsole = true;
                    else if(h == Hashes::InitApp)
                        Database::InitApp = true;
                    else
                    {
                        IsError = true;
                        ep.Line = Line;
                        ep.Character = Character;
                        ep.Message = "The keyword '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + " is unrecognized";
                    }
                }
            }
            else if (IsType)
            {
                IsError = true;
                ep.Line = Line;
                ep.Character = Character;
                ep.Message = "Expected the project type specifier after the 'type' command starting at " + to_string(LastLine) + " : " + to_string(LastCharacter);
            }
            else if(IsIgnore)
            {
                IsError = true;
                ep.Line = Line;
                ep.Character = Character;
                ep.Message = "Expected the path of the ignored iris file after the 'ignore' specifier starting at " + to_string(LastLine) + " : " + to_string(LastCharacter);
            }
            else if(IsNatlib)
            {
                IsError = true;
                ep.Line = Line;
                ep.Character = Character;
                ep.Message = "Expected the path of a native library after the 'natlib' command starting at " + to_string(LastLine) + " : " + to_string(LastCharacter);
            }
            else if(IsMeasureType)
            {
                IsError = true;
                ep.Line = Line;
                ep.Character = Character;
                ep.Message = "Expected the type of time measurement after the 'measuretype' command starting at " + to_string(LastLine) + " : " + to_string(LastCharacter);
            }
            Database::IrisFiles = Utilities::GetFilesInFolder(Database::SourceFolder);
            Console::Append("Reading and parsing the eye project file took " + Utilities::TimeDiff(a, Utilities::TimePoint()));
            if(Database::ProjectName.size() == 0 && !InvalidCharName)
                Console::Append("\n\nError at '" + Database::ProjectPath + "' -> The project name is missing");
            else if(IsError)
                Console::Append("\n\nError at '" + Database::ProjectPath + "', " + to_string(ep.Line) + " : " + to_string(ep.Character) + " -> " + ep.Message);
            else if(Database::IrisFiles.size() == 0)
                Console::Append("\n\nThere are no source files with .iris extension");
            else
            {
                if(!Database::UseVisualDebugger)
                {
                    if(Database::NativeLibraries.size() > 0)
                    {
                        thread t(Linker::Process);
                        Pupil::Process();
                        t.join();
                    }
                    else
                        Pupil::Process();
                    if(!Database::LinkerError && !Database::PupilError)
                    {
                        Nerve::Process();
                        Console::Append("\n\nThe entire compilation took " + Utilities::TimeDiff(a, Utilities::TimePoint()));
                        Utilities::WriteBinary(Database::OutputFolder + Database::ProjectName + ".exe", Database::Binary);
                    }
                }
                else if(Database::NativeLibraries.size() > 0)
                {
                    thread t(Linker::Process);
                    thread tt(Pupil::Process);
                    VisualDebugger::Start();
                    t.join();
                    tt.join();
                    if(!Database::LinkerError && !Database::PupilError)
                    {
                        Nerve::Process();
                        Console::Append("\n\nThe entire compilation took " + Utilities::TimeDiff(a, Utilities::TimePoint()));
                        Utilities::WriteBinary(Database::OutputFolder + Database::ProjectName + ".exe", Database::Binary);
                    }
                }
                else
                {
                    thread t(Pupil::Process);
                    VisualDebugger::Start();
                    t.join();
                    if(!Database::PupilError)
                    {
                        Nerve::Process();
                        Console::Append("\n\nThe entire compilation took " + Utilities::TimeDiff(a, Utilities::TimePoint()));
                        Utilities::WriteBinary(Database::OutputFolder + Database::ProjectName + ".exe", Database::Binary);
                    }
                }
            }
            if(Database::ReportPupil)
                Pupil::Report();
            Database::LastMessage = true;
        }
    }
}