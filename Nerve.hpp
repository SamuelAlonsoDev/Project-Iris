//Copyright 2020 Iris Technologies, All Rights Reserved
#pragma once
#include <Linker.hpp>
#include <X64.hpp>

namespace ProjectIris
{
	namespace Nerve
	{
		//Source Register values -> 0 = nothing, 1 = al
		//Destination Register(Future use) values -> 0 = nothing, 1 = al

		__forceinline void Process() noexcept
		{
			vector<RegisterAliasing> Registers;
			RegisterAliasing ra;
			size_t VarSize = 0;
			bool BusyAl = false;//Indicates if 'al' register is being used
			for(size_t x = 0; x < Database::PupilFInstructions.size(); ++x)
			{
				if(Database::PupilFInstructions[x].Opcode == Opcodes::LocalVariableType)
				{
					if(Database::PupilFInstructions[x].Name == Hashes::S8)
						VarSize = 8;
				}
				else if(Database::PupilFInstructions[x].Opcode == Opcodes::LocalVariableName)
				{
					if(VarSize == 8)
					{
						if(!BusyAl)//al register isn't being used so we can alias the name
						{
							BusyAl = true;
							ra.VarName = Database::PupilFInstructions[x].Name;
							ra.Register = 1;
							Registers.push_back(ra);
						}
					}
				}
				else if(Database::PupilFInstructions[x].Opcode == Opcodes::EqualOperation)
				{
					size_t ActualRegisterAliasing = 0;
					for(size_t y = 0; y < Registers.size(); ++y)
						if(Database::PupilFInstructions[x - 1].Name == Registers[y].VarName)
						{
							ActualRegisterAliasing = y;
							break;
						}
					if(Registers[ActualRegisterAliasing].Register == 1 && Database::PupilFInstructions[x + 1].IsLiteral)
					{
						Database::Text.push_back(X64::Mov8ImmToAl);
						Database::Text.push_back(static_cast<unsigned char>(Database::PupilFInstructions[x + 1].Name));
						Registers.erase(Registers.begin() + ActualRegisterAliasing);
					}
				}
			}
			if(Database::os == OperatingSystem::Windows || Database::os == OperatingSystem::Xbox)
				Linker::LinkPE();
		}
	}
}