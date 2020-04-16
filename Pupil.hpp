//Copyright 2020 Iris Technologies, All Rights Reserved
#pragma once
#include <Utilities.hpp>
#include <Console.hpp>
#include <thread>
#include <sstream>

namespace ProjectIris
{
    namespace Pupil
    {
        __forceinline void Report() noexcept
        {
            string Output("New file " + Database::PupilFiles[0]);
            for(size_t x = 1; x < Database::PupilFInstructions.size(); ++x)
                switch (Database::PupilFInstructions[x].Opcode)
                {
                case Opcodes::NewFile:
                    Output.append("\n\nNew file " + Database::PupilFiles[Database::PupilFInstructions[x].Name]);
                    break;
                case Opcodes::StartModule:
                    Output.append("\n\nStart module '" + to_string(Database::PupilFInstructions[x].Name) + "'");
                    break;
                case Opcodes::EndModule:
                    Output.append("\n\nEnd module '" + to_string(Database::PupilFInstructions[x].Name) + "'");
                    break;
                case Opcodes::StartFunction:
                    Output.append("\n\nStart function '" + to_string(Database::PupilFInstructions[x].Name) + "'");
                    break;
                case Opcodes::InArgument:
                    Output.append("\n\nInput argument of type '");
                    break;
                case Opcodes::InoutArgument:
                    Output.append("\n\nInput/output argument of type '");
                    break;
                case Opcodes::OutArgument:
                    Output.append("\n\nOutput argument of type '");
                    break;
                case Opcodes::ArgumentType:
                    Output.append(to_string(Database::PupilFInstructions[x].Name) + "' with name '");
                    break;
                case Opcodes::ArgumentName:
                    Output.append(to_string(Database::PupilFInstructions[x].Name) + "'");
                    break;
                case Opcodes::Main:
                    Output.append("\n\nStarting main function...");
                    break;
                case Opcodes::FunctionBody:
                    Output.append("\n\nStarting function body...");
                    break;
                case Opcodes::EndFunction:
                    Output.append("\n\nEnding function");
                    break;
                case Opcodes::LocalVariableType:
                    Output.append("\n\nVariable type: " + to_string(Database::PupilFInstructions[x].Name) + ", Variable name: ");
                    break;
                case Opcodes::LocalVariableName:
                    Output.append(to_string(Database::PupilFInstructions[x].Name));
                    break;
                case Opcodes::LvalueKeyword:
                    Output.append("\n\nLvalue keyword '" + to_string(Database::PupilFInstructions[x].Name) + "' ");
                    break;
                case Opcodes::EqualOperation:
                    Output.append("is equal to Rvalue keyword '");
                    break;
                case Opcodes::RvalueKeyword:
                    Output.append(to_string(Database::PupilFInstructions[x].Name) + "'");
                    break;
                case Opcodes::Using:
                    Output.append("\n\nUsing directive '" + to_string(Database::PupilFInstructions[x].Name) + "'");
                    break;
                }
            Utilities::WriteFile(Database::OutputFolder + Database::ProjectName + ".pupil", Output);
        }

        __forceinline void Process() noexcept
        {
            PupilFInstruction ins;
            for(size_t x = 0; x < Database::IrisFiles.size(); ++x)
            {
                string Name = Database::IrisFiles[x];
                Name.erase(0, Database::ProjectSourceLength);
                if(Database::UseVisualDebugger && !Database::PupilError)
                    Database::ActualFile = Name;
                Database::PupilFiles.push_back(Name);
                ins.Opcode = Opcodes::NewFile;
                Database::PupilFInstructions.push_back(ins);
                string Keyword, Modules;
                size_t Line = 1, Character = 0, LastLine, LastCharacter, ModuleCount = 0, ActualModule = 0, FunctionNode = 0, LvalueHash = 0, SleepTime = 0, ActualClass, MainFile, MainLine, MainCharacter;
                char Mode = 0;
                bool UseVDBG = Database::UseVisualDebugger, FirstChar = false, FirstSpace = false, IsModule = false, IsFunction = false, IsFunctionArguments = false, IsFunctionBody = false, IsLocalVariableDeclaration = true, IsMacro = false, IsSlowdown = false, IsUsing = false, IsClass = false, IsMain = false, MainFound = false;
                chrono::steady_clock::time_point a = Utilities::TimePoint();
                const string Content = Utilities::ReadFile(Database::IrisFiles[x]);
                for(size_t i = 0; i < Content.size(); ++i)
                {
                    if(SleepTime != 0)
                        this_thread::sleep_for(chrono::milliseconds(SleepTime));
                    if(Content[i] == '\n')
                    {
                        if(UseVDBG)
                        {
                            ++Database::ActualLine;
                            Database::ActualCharacter = 0;
                        }
                        ++Line;
                        Character = 0;
                    }
                    else
                    {
                        if(UseVDBG)
                            ++Database::ActualCharacter;
                        ++Character;
                    }
                    if(IsFunctionBody)
                    {
                        if(Content[i] == '}')
                        {
                            if(FunctionNode == 0)
                            {
                                FirstSpace = false;
                                IsLocalVariableDeclaration = true;
                                IsFunctionBody = false;
                                ins.Opcode = Opcodes::EndFunction;
                                Database::PupilFInstructions.push_back(ins);
                            }
                            else
                                --FunctionNode;
                        }
                        //I know it is bad but i just wanted to have something to do the backend, it will be refactored
                        else if(Content[i] == ';')
                        {
                            if(Keyword.size() > 0)
                            {
                                if(!IsLocalVariableDeclaration)
                                {
                                    FirstSpace = false;
                                    if (Utilities::nIsOnlyNumeric(Keyword.c_str(), Keyword.size()))
                                    {
                                        stringstream ss(Keyword);
                                        ss >> ins.Name;
                                        ins.IsLiteral = true;
                                    }
                                    else
                                        ins.Name = Utilities::Hash(Keyword);
                                    Keyword.clear();
                                    ins.Opcode = Opcodes::RvalueKeyword;
                                    Database::PupilFInstructions.push_back(ins);
                                }
                                else if(FirstSpace)
                                {
                                    FirstSpace = false;
                                    ins.Name = LvalueHash;
                                    LvalueHash = 0;
                                    ins.Opcode = Opcodes::LocalVariableType;
                                    Database::PupilFInstructions.push_back(ins);
                                    ins.Name = Utilities::Hash(Keyword);
                                    Keyword.clear();
                                    ins.Opcode = Opcodes::LocalVariableName;
                                    Database::PupilFInstructions.push_back(ins);
                                }
                            }
                        }
                        else if(Content[i] == '=')
                        {
                            if(Keyword.size() > 0)
                            {
                                IsLocalVariableDeclaration = false;
                                ins.Name = Utilities::Hash(Keyword);
                                Keyword.clear();
                                ins.Opcode = Opcodes::LvalueKeyword;
                                Database::PupilFInstructions.push_back(ins);
                                ins.Opcode = Opcodes::EqualOperation;
                                Database::PupilFInstructions.push_back(ins);
                            }
                            else if(LvalueHash != 0)
                            {
                                IsLocalVariableDeclaration = false;
                                ins.Name = LvalueHash;
                                LvalueHash = 0;
                                ins.Opcode = Opcodes::LvalueKeyword;
                                Database::PupilFInstructions.push_back(ins);
                                ins.Opcode = Opcodes::EqualOperation;
                                Database::PupilFInstructions.push_back(ins);
                            }
                            else
                            {
                                const string msg("The lvalue is missing");
                                Utilities::Report(Name, Line, Character, msg);
                                break;
                            }
                        }
                        else if(Content[i] != ' ' && Content[i] != '\n')
                        {
                            if(!Utilities::IsAlphanumeric(Content[i]))
                            {
                                const string c(1, Content[i]);
                                const string msg("The character '" + c + "' is invalid");
                                Utilities::Report(Name, Line, Character, msg);
                                break;
                            }
                            else if(!FirstChar)
                            {
                                FirstChar = true;
                                LastLine = Line;
                                LastCharacter = Character;
                                Keyword.push_back(Content[i]);
                            }
                            else
                                Keyword.push_back(Content[i]);
                        }
                        else if(Keyword.size() > 0 && !FirstSpace)
                        {
                            FirstSpace = true;
                            LvalueHash = Utilities::Hash(Keyword);
                            Keyword.clear();
                        }
                    }
                    else if(IsFunctionArguments)
                    {
                        if(Content[i] == '{')
                        {
                            if(FirstSpace)
                            {
                                FirstSpace = false;
                                FirstChar = false;
                                IsFunctionArguments = false;
                                IsFunctionBody = true;
                                ins.Opcode = Opcodes::FunctionBody;
                                Database::PupilFInstructions.push_back(ins);
                            }
                            else
                            {
                                const string msg("Expected ')' before '{'");
                                Utilities::Report(Name, Line, Character, msg);
                                break;
                            }
                        }
                        else if(Content[i] == ' ' || Content[i] == '\n')
                        {
                            if(Keyword.size() > 0)
                            {
                                if(Mode == 0)
                                {
                                    ++Mode;
                                    FirstChar = false;
                                    const size_t h = Utilities::Hash(Keyword);
                                    Keyword.clear();
                                    if(h == Hashes::In)
                                    {
                                        ins.Opcode = Opcodes::InArgument;
                                        Database::PupilFInstructions.push_back(ins);
                                    }
                                    else if(h == Hashes::Inout)
                                    {
                                        ins.Opcode = Opcodes::InoutArgument;
                                        Database::PupilFInstructions.push_back(ins);
                                    }
                                    else if(h == Hashes::Out)
                                    {
                                        ins.Opcode = Opcodes::OutArgument;
                                        Database::PupilFInstructions.push_back(ins);
                                    }
                                    else
                                    {
                                        const string msg("Invalid argument direction '" + Keyword + "' starting at " + to_string(LastLine) + " : " + Utilities::to_stringc(LastCharacter) + ". Valid argument directions are 'in', 'inout', and 'out'");
                                        Utilities::Report(Name, Line, Character, msg);
                                        break;
                                    }
                                }
                                else if(Mode == 1)
                                {
                                    ++Mode;
                                    FirstChar = false;
                                    ins.Name = Utilities::Hash(Keyword);
                                    Keyword.clear();
                                    ins.Line = LastLine;
                                    ins.Character = LastCharacter;
                                    ins.Opcode = Opcodes::ArgumentType;
                                    Database::PupilFInstructions.push_back(ins);
                                }
                                else if(Mode == 2)
                                {
                                    if(Utilities::nIsOnlyNumeric(Keyword.c_str(), Keyword.size()))
                                    {
                                        const string msg("The argument name '" + Keyword + "' starting at " + to_string(LastLine) + " : " + Utilities::to_stringc(LastCharacter) + " is only numeric");
                                        Utilities::Report(Name, Line, Character, msg);
                                        break;
                                    }
                                    else
                                    {
                                        ++Mode;
                                        ins.Name = Utilities::Hash(Keyword);
                                        Keyword.clear();
                                        ins.Line = LastLine;
                                        ins.Character = LastCharacter;
                                        ins.Opcode = Opcodes::ArgumentName;
                                        Database::PupilFInstructions.push_back(ins);
                                    }
                                }
                            }
                        }
                        else if(FirstSpace)
                        {
                            const string msg("Expected '{'");
                            Utilities::Report(Name, Line, Character, msg);
                            break;
                        }
                        else if(Content[i] == ',')
                        {
                            if(Mode == 3)
                                Mode = 0;
                            else if(Mode == 2)
                            {
                                if(Keyword.size() > 0)
                                {
                                    if(Utilities::nIsOnlyNumeric(Keyword.c_str(), Keyword.size()))
                                    {
                                        const string msg("The argument name '" + Keyword + "' starting at " + to_string(LastLine) + " : " + Utilities::to_stringc(LastCharacter) + " is only numeric");
                                        Utilities::Report(Name, Line, Character, msg);
                                        break;
                                    }
                                    else
                                    {
                                        Mode = 0;
                                        ins.Name = Utilities::Hash(Keyword);
                                        Keyword.clear();
                                        ins.Opcode = Opcodes::ArgumentName;
                                        Database::PupilFInstructions.push_back(ins);
                                    }
                                }
                                else
                                {
                                    const string msg("Expected the argument name after the argument type and before ','");
                                    Utilities::Report(Name, Line, Character, msg);
                                    break;
                                }
                            }
                            else if(Mode == 1)
                            {
                                const string msg("Expected the argument type after the argument direction and before ','");
                                Utilities::Report(Name, Line, Character, msg);
                                break;
                            }
                            else
                            {
                                const string msg("Expected the argument direction before ','");
                                Utilities::Report(Name, Line, Character, msg);
                                break;
                            }
                        }
                        else if(Content[i] == ')')
                        {
                            if(Mode == 3)
                            {
                                Mode = 0;
                                FirstSpace = true;
                            }
                            else if(Mode == 2)
                            {
                                if(Keyword.size() > 0)
                                {
                                    if(Utilities::nIsOnlyNumeric(Keyword.c_str(), Keyword.size()))
                                    {
                                        const string msg("The argument name '" + Keyword + "' starting at " + to_string(LastLine) + " : " + Utilities::to_stringc(LastCharacter) + " is only numeric");
                                        Utilities::Report(Name, Line, Character, msg);
                                        break;
                                    }
                                    else
                                    {
                                        Mode = 0;
                                        FirstSpace = true;
                                        ins.Name = Utilities::Hash(Keyword);
                                        Keyword.clear();
                                        ins.Opcode = Opcodes::ArgumentName;
                                        Database::PupilFInstructions.push_back(ins);
                                    }
                                }
                                else
                                {
                                    const string msg("Expected the argument name after the argument type and before ')'");
                                    Utilities::Report(Name, Line, Character, msg);
                                    break;
                                }
                            }
                            else if(Mode == 1)
                            {
                                const string msg("Expected the argument type after the argument direction and before ')'");
                                Utilities::Report(Name, Line, Character, msg);
                                break;
                            }
                            else
                                FirstSpace = true;
                        }
                        else if(!FirstChar)
                        {
                            FirstChar = true;
                            LastLine = Line;
                            LastCharacter = Character;
                            Keyword.push_back(Content[i]);
                        }
                        else
                            Keyword.push_back(Content[i]);
                    }
                    else if(IsFunction)
                    {
                        if(Content[i] == '(')
                        {
                            if(Utilities::nIsOnlyNumeric(Keyword.c_str(), Keyword.size()))
                            {
                                const string msg("The function name '" + Keyword + "' starting at " + to_string(LastLine) + " : " + Utilities::to_stringc(LastCharacter) + " is only numeric");
                                Utilities::Report(Name, Line, Character, msg);
                                break;
                            }
                            else
                            {
                                ins.Name = Utilities::Hash(Keyword);
                                Keyword.clear();
                                ins.Module = ActualModule;
                                ins.Opcode = Opcodes::StartFunction;
                                Database::PupilFInstructions.push_back(ins);
                                IsFunction = false;
                                FirstSpace = false;
                                FirstChar = false;
                                IsFunctionArguments = true;
                            }
                        }
                        else if(Content[i] == ' ' || Content[i] == '\n')
                        {
                            if(Keyword.size() > 0)
                                FirstSpace = true;
                        }
                        else if(FirstSpace)
                        {
                            const string c(1, Content[i]);
                            const string msg("Expected '(', found '" + c + "'");
                            Utilities::Report(Name, Line, Character, msg);
                            break;
                        }
                        else if(!Utilities::IsAlphanumeric(Content[i]))
                        {
                            const string c(1, Content[i]);
                            const string msg("Invalid character '" + c + "'. Valid characters for a function name are '_', from '0' to '9', from 'A' to 'Z', and from 'a' to 'z'");
                            Utilities::Report(Name, Line, Character, msg);
                            break;
                        }
                        else if(!FirstChar)
                        {
                            FirstChar = true;
                            LastLine = Line;
                            LastCharacter = Character;
                            Keyword.push_back(Content[i]);
                        }
                        else
                            Keyword.push_back(Content[i]);
                    }
                    else if(IsMain)
                    {
                        if(Content[i] != ' ' && Content[i] != '\n')
                        {
                            if(Content[i] == '{')
                            {
                                IsMain = false;
                                MainFound = true;
                                IsFunctionBody = true;
                                MainFile = x;
                                MainLine = LastLine;
                                MainCharacter = LastCharacter;
                                ins.Opcode = Opcodes::Main;
                                Database::PupilFInstructions.push_back(ins);
                            }
                            else
                            {
                                const string msg("Expected '{'");
                                Utilities::Report(Name, Line, Character, msg);
                                break;
                            }
                        }
                    }
                    else if(IsClass)
                    {
                        if(Content[i] == '{')
                        {
                            if(Utilities::nIsOnlyNumeric(Keyword.c_str(), Keyword.size()))
                            {
                                //error
                            }
                            else
                            {
                                IsClass = false;
                                FirstChar = false;
                                FirstSpace = false;
                                ActualClass = Utilities::Hash(Keyword);
                                Keyword.clear();
                                ins.Name = ActualClass;
                                ins.Opcode = Opcodes::StartClass;
                                Database::PupilFInstructions.push_back(ins);
                            }
                        }
                        else if(Content[i] == ' ' || Content[i] == '\n')
                        {
                            if(Keyword.size() > 0)
                                FirstSpace = true;
                        }
                        else if(FirstSpace)
                        {
                            //error
                        }
                        else if(!Utilities::IsAlphanumeric(Content[i]))
                        {
                            //error
                        }
                        else if(!FirstChar)
                        {
                            FirstChar = true;
                            LastLine = Line;
                            LastCharacter = Character;
                            Keyword.push_back(Content[i]);
                        }
                        else
                            Keyword.push_back(Content[i]);
                    }
                    else if(IsUsing)
                    {
                        if(Content[i] == ';')
                        {
                            if(Utilities::nIsOnlyNumeric(Keyword.c_str(), Keyword.size()))
                            {
                                const string msg("The using directive '" + Keyword + "' starting at " + to_string(LastLine) + " : " + Utilities::to_stringc(LastCharacter) + " is only numeric");
                                Utilities::Report(Name, Line, Character, msg);
                                break;
                            }
                            else
                            {
                                IsUsing = false;
                                FirstChar = false;
                                FirstSpace = false;
                                ins.Name = Utilities::Hash(Keyword);
                                Keyword.clear();
                                ins.Opcode = Opcodes::Using;
                                Database::PupilFInstructions.push_back(ins);
                            }
                        }
                        else if(Content[i] == ' ' || Content[i] == '\n')
                        {
                            if(Keyword.size() > 0)
                                FirstSpace = true;
                        }
                        else if(FirstSpace)
                        {
                            const string c(1, Content[i]);
                            const string msg("Expected ';', but found '" + c + "'");
                            Utilities::Report(Name, Line, Character, msg);
                            break;
                        }
                        else if(!Utilities::IsAlphanumeric(Content[i]))
                        {
                            const string c(1, Content[i]);
                            const string msg("Invalid character '" + c + "', it's not alphanumeric. Valid characters are '_', from '0' to '9', from 'A' to 'Z', and from 'a' to 'z'");
                            Utilities::Report(Name, Line, Character, msg);
                            break;
                        }
                        else if(!FirstChar)
                        {
                            FirstChar = true;
                            LastLine = Line;
                            LastCharacter = Character;
                            Keyword.push_back(Content[i]);
                        }
                        else
                            Keyword.push_back(Content[i]);
                    }
                    else if(IsSlowdown)
                    {
                        if(Content[i] == ' ' || Content[i] == '\n')
                        {
                            if(Keyword.size() > 0)
                            {
                                IsSlowdown = false;
                                if(Database::UseVisualDebugger)
                                {
                                    const size_t h = Utilities::Hash(Keyword);
                                    if(Utilities::IsOnlyNumeric(Keyword))
                                    {
                                        stringstream ss(Keyword);
                                        Keyword.clear();
                                        ss >> SleepTime;
                                    }
                                    else if(h == Hashes::Off)
                                    {
                                        Keyword.clear();
                                        SleepTime = 0;
                                    }
                                    else
                                    {
                                        const string msg("The slowdown amount '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter) + ", is not only integer nor positive");
                                        Utilities::Report(Name, Line, Character, msg);
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                const string msg("The time to slowdown is not specified");
                                Utilities::Report(Name, Line, Character, msg);
                                break;
                            }
                        }
                        else if(!FirstChar)
                        {
                            FirstChar = true;
                            LastLine = Line;
                            LastCharacter = Character;
                            Keyword.push_back(Content[i]);
                        }
                        else
                            Keyword.push_back(Content[i]);
                    }
                    else if(IsModule)
                    {
                        if(Content[i] == '{')
                        {
                            if(Utilities::nIsOnlyNumeric(Keyword.c_str(), Keyword.size()))
                            {
                                const string msg("The module name '" + Keyword + "' starting at " + to_string(LastLine) + " : " + Utilities::to_stringc(LastCharacter) + " is only numeric");
                                Utilities::Report(Name, Line, Character, msg);
                                break;
                            }
                            else
                            {
                                if(ModuleCount > 0)
                                    Modules.append(".");
                                ++ModuleCount;
                                Modules.append(Keyword);
                                Keyword.clear();
                                ActualModule = Utilities::Hash(Modules);
                                ins.Name = ActualModule;
                                ins.Opcode = Opcodes::StartModule;
                                Database::PupilFInstructions.push_back(ins);
                                IsModule = false;
                                FirstChar = false;
                                FirstSpace = false;
                            }
                        }
                        else if(Content[i] == ' ' || Content[i] == '\n')
                        {
                            if(Keyword.size() > 0)
                                FirstSpace = true;
                        }
                        else if(FirstSpace)
                        {
                            const string c(1, Content[i]);
                            const string msg("Expected '{', found '" + c + "'");
                            Utilities::Report(Name, Line, Character, msg);
                            break;
                        }
                        else if(!Utilities::IsAlphanumeric(Content[i]))
                        {
                            const string c(1, Content[i]);
                            const string msg("Invalid character '" + c + "'. Valid characters for a module name are '_', from '0' to '9', from 'A' to 'Z', and from 'a' to 'z'");
                            Utilities::Report(Name, Line, Character, msg);
                            break;
                        }
                        else if(!FirstChar)
                        {
                            FirstChar = true;
                            LastLine = Line;
                            LastCharacter = Character;
                            Keyword.push_back(Content[i]);
                        }
                        else
                            Keyword.push_back(Content[i]);
                    }
                    else if(Content[i] == ' ' || Content[i] == '\n')
                    {
                        if (Keyword.size() > 0)
                        {
                            FirstChar = false;
                            if(IsMacro)
                            {
                                IsMacro = false;
                                const size_t h = Utilities::Hash(Keyword);
                                Keyword.clear();
                                if(h == Hashes::Sldown)
                                    IsSlowdown = true;
                            }
                            else
                            {
                                const size_t h = Utilities::Hash(Keyword);
                                Keyword.clear();
                                if(h == Hashes::Module)
                                    IsModule = true;
                                else if(h == Hashes::Function)
                                    IsFunction = true;
                                else if(h == Hashes::Using)
                                    IsUsing = true;
                                else if(h == Hashes::Class)
                                    IsClass = true;
                                else if(h == Hashes::Main)
                                {
                                    if(!MainFound)
                                        IsMain = true;
                                    else
                                    {
                                        const string msg("Main is already loaded at the file '" + Database::PupilFiles[MainFile] + "', " + to_string(MainLine) + " : " + Utilities::to_stringc(MainCharacter));
                                        Utilities::Report(Name, Line, Character, msg);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    else if(Content[i] == '#')
                        IsMacro = true;
                    else if(Content[i] == '}')
                    {
                        if(ModuleCount == 0)
                        {
                            const string msg("Invalid '}'");
                            Utilities::Report(Name, Line, Character, msg);
                            break;
                        }
                        else if(Keyword.size() > 0)
                        {
                            const string msg("Invalid keyword '" + Keyword + "' starting at " + to_string(LastLine) + " : " + to_string(LastCharacter));
                            Utilities::Report(Name, Line, Character, msg);
                            break;
                        }
                        else if(ModuleCount > 1)
                        {
                            --ModuleCount;
                            ActualModule = Utilities::Hash(Modules);
                            ins.Name = ActualModule;
                            ins.Opcode = Opcodes::EndModule;
                            Database::PupilFInstructions.push_back(ins);
                            size_t xx = Modules.size() - 1;
                            for(; xx > 0; --xx)
                                if(Modules[xx] == '.')
                                    break;
                            Modules.erase(xx, Modules.size() - xx);
                        }
                        else
                        {
                            ModuleCount = 0;
                            ActualModule = Utilities::Hash(Modules);
                            ins.Name = ActualModule;
                            ins.Opcode = Opcodes::EndModule;
                            Database::PupilFInstructions.push_back(ins);
                            Modules.clear();
                        }
                    }
                    else if(!Utilities::IsAlphanumeric(Content[i]))
                    {
                        const string c(1, Content[i]);
                        const string msg("Invalid character '" + c + "'. Valid characters are '_', from '0' to '9', from 'A' to 'Z', and from 'a' to 'z'");
                        Utilities::Report(Name, Line, Character, msg);
                        break;
                    }
                    else if(!FirstChar)
                    {
                        FirstChar = true;
                        LastLine = Line;
                        LastCharacter = Character;
                        Keyword.push_back(Content[i]);
                    }
                    else
                        Keyword.push_back(Content[i]);
                }
                if(Keyword.size() > 0)
                {
                    if(IsModule)
                    {
                        const string msg("Expected '{' after the module name '" + Keyword + "' starting at " + to_string(LastLine) + " : " + Utilities::to_stringc(LastCharacter));
                        Utilities::Report(Name, Line, Character, msg);
                    }
                    else
                    {
                        const string msg("Invalid keyword '" + Keyword + "' starting at " + to_string(LastLine) + " : " + Utilities::to_stringc(LastCharacter) + " is invalid");
                        Utilities::Report(Name, Line, Character, msg);
                    }
                }
                else if(ModuleCount > 0)
                {
                    const string msg("Expected '}'");
                    Utilities::Report(Name, Line, Character, msg);
                }
                else if(IsModule)
                {
                    const string msg("Expected the module name after the 'module' specifier starting at " + to_string(LastLine) + " : " + Utilities::to_stringc(LastCharacter));
                    Utilities::Report(Name, Line, Character, msg);
                }
                const chrono::steady_clock::time_point b = Utilities::TimePoint();
                const string s = Utilities::TimeDiff(a, b);
                Console::Append("\n\nReading and parsing the iris file '" + Name + "' took " + s);
            }
        }
    }
}