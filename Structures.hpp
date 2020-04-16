//Copyright 2020 Iris Technologies, All Rights Reserved
#pragma once
#include <string>
#include <vector>

using namespace std;

namespace ProjectIris
{
	namespace Hashes
	{
		constexpr size_t InitApp = 4229616916362953318;//initapp

		constexpr size_t IsConsole = 4280226163429640506;//isconsole

		constexpr size_t Natlib = 2283687954489727723;//natlib

		constexpr size_t Ignore = 12181781538219395523;//ignore

		constexpr size_t UseVisualDebugger = 4970868739992279183;//usevisualdebugger

		constexpr size_t MeasureType = 9985186892661743463;//measuretype

		constexpr size_t ReportPupil = 15381437785649700129;//reportpupil

		constexpr size_t Name = 14176396743819860870;//name

		constexpr size_t Type = 12075340201627130925;//type

		constexpr size_t Executable = 13678059843563106925;//executable

		constexpr size_t Library = 16152639334691645584;//library

		constexpr size_t Sandbox = 3331646911590050372;//sandbox

		constexpr size_t Nanoseconds = 626911876451167078;//ns

		constexpr size_t Microseconds = 582730275372608502;//mcs

		constexpr size_t Milliseconds = 624114718869565969;//ms

		constexpr size_t Off = 1883801076392705514;//off

		constexpr size_t Sldown = 5780042772118102530;//sldown

		constexpr size_t Stop = 12618894948741332165;//stop
	
		constexpr size_t Module = 4892776386767805149;//module

		constexpr size_t Using = 15689622396028117031;//using

		constexpr size_t Function = 619343937915704565;//fn

		constexpr size_t Main = 2258945139493307336;//main

		constexpr size_t In = 628032278800124862;//in

		constexpr size_t Out = 1871134702438174719;//out

		constexpr size_t Inout = 4023996569761782818;//inout

		constexpr size_t Const = 7334518323283222324;//const

		constexpr size_t Class = 15066323702654938015;//class

		constexpr size_t OperatorPlus = 5084307088801188994;//operator+

		constexpr size_t OperatorMinus = 5084300491731419728;//operator-

		constexpr size_t OperatorEqual = 5084282899545368352;//operator=

		constexpr size_t OperatorMultiplication = 5084308188312817205;//operator*

		constexpr size_t OperatorDivision = 5084302690754676150;//operator/

		constexpr size_t OperatorIncrement = 16424530381718799147;//operator++

		constexpr size_t OperatorDecrement = 16418797528090442343;//operator--

		constexpr size_t OperatorPlusEqual = 16424554570974619789;//operator+=

		constexpr size_t OperatorMinusEqual = 16418779935904390967;//operator-=

		constexpr size_t OperatorMultiplicationEqual = 16425394597858383768; //operator*=

		constexpr size_t OperatorDivisionEqual = 16420613921299868465;//operator/=

		constexpr size_t Equal = 12638138822044000712;//=

		constexpr size_t S8 = 637547452428770606;//s8
	}

	namespace Opcodes
        {
            constexpr unsigned short Nothing = 0;

            constexpr unsigned short NewFile = 1;

            constexpr unsigned short StartModule = 2;
            
            constexpr unsigned short EndModule = 3;

            constexpr unsigned short Using = 4;

            constexpr unsigned short StartFunction = 5;

            constexpr unsigned short EndFunction = 6;

            constexpr unsigned short InArgument = 7;

            constexpr unsigned short InoutArgument = 8;

            constexpr unsigned short OutArgument = 9;

            constexpr unsigned short ArgumentType = 10;

            constexpr unsigned short ArgumentName = 11;

            constexpr unsigned short FunctionBody = 12;

            constexpr unsigned short LvalueKeyword = 13;

            constexpr unsigned short EqualOperation = 14;

            constexpr unsigned short RvalueKeyword = 15;

            constexpr unsigned short LocalVariableType = 16;

            constexpr unsigned short LocalVariableName = 17;

        constexpr unsigned short StartClass = 18;

        constexpr unsigned short Main = 19;
    }

	struct Bytes8
	{
		unsigned char a, b, c, d, e, f, g, h;
	};

	struct Bytes4
	{
		unsigned char a, b, c, d;
	};

	struct Bytes2
	{
		unsigned char a, b;
	};

	struct PESection
	{
		unsigned int PointerToRawData, VirtualEnd, VirtualAddress;//VirtualEnd = VirtualAddress + VirtualSize
	};

	struct PENativeSection
	{
		unsigned int PointerToRawData, SizeOfRawData, VirtualAddress, VirtualSize;
	};

	enum class OperatingSystem : unsigned char
	{
		Windows,
		Xbox,
		
	};

	struct ErrorPoint
	{
		size_t Line, Character;

		string File, Message;

		__forceinline void clear() noexcept
		{
			File.clear();
			Message.clear();
		}
	};

	struct NativeLibrary
	{
		size_t Line, Character;

		string Name;
	};

	enum class MeasureTypes : unsigned char
	{
		Nanoseconds,
		Microseconds,
		Milliseconds
	};

	enum class ProjectType : unsigned char
	{
		Sandbox,
		Library,
		Executable
	};

	struct IgnoredFile
	{
		size_t Line, Character;

		string Name;
	};

	enum class ArgumentDirection : unsigned char
	{
		In,
		Out,
		Inout
	};

	struct Argument
	{
		size_t Type;

		ArgumentDirection Direction;
	};

	struct FunctionPrototype
	{
		size_t Name, Character, Line;
		
		vector<Argument> Arguments;
	};

	struct PupilFInstruction
	{
		bool IsLiteral = false;

		unsigned short Opcode;

		size_t Pointer, Module, Name, Line, Character;
	};

	struct RegisterAliasing
	{
		size_t VarName;

		char Register;
	};
}