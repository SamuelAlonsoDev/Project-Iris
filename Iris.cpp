//Copyright 2020 Iris Technologies, All Rights Reserved
#pragma once
#include <Eye.hpp>
#include <Utilities.hpp>
#include <Console.hpp>
#include <Iris.hpp>
#include <cstdlib>

void Execute() noexcept
{
	system("cls");//I know, i know, it won't be on the final version, it's just that i didn't make the Win32 function work, and
	//i had some other priorities, so i just leave that by now, don't blame me too much
	thread t(ProjectIris::Eye::Process);
	bool Exit = false;
	while (!Exit)
	{
		this_thread::sleep_for(chrono::milliseconds(200));
		if (ProjectIris::Database::LastMessage)
			Exit = true;
	}
	ProjectIris::Utilities::Recompile();
}

int main(int argc, char** argv)
{
	system("cls");
	HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, 1200, 600, TRUE);
	SetConsoleTitle("Alice");
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, 11 | FOREGROUND_INTENSITY);
	const string e1("Wrong number of arguments!");
	if (argc == 3)
	{
		const string cmd(argv[1]);
		const string s(argv[2]);
		const string e2("The file '" + s + "' does not exists!");
		if(cmd == "compile")
		{
			if(ProjectIris::Utilities::FileExists(s))
			{
				const string base = ProjectIris::Utilities::GetDirectory(s) + "\\";
				const string src = base + "src";
				const string out = base + "output";
				const string e3("The source folder '" + src + "' does not exists!");
				const string e4("The output folder '" + out + "' does not exists!");
				if(!ProjectIris::Utilities::DirectoryExists(src))
					std::cout << e3 << std::endl;
				else if(!ProjectIris::Utilities::DirectoryExists(out))
					std::cout << e4 << std::endl;
				else
				{
					ProjectIris::Database::ProjectSourceLength = src.size() + 1;
					ProjectIris::Database::ProjectPath = s;
					ProjectIris::Database::SourceFolder = src;
					ProjectIris::Database::OutputFolder = out + "\\";
					Execute();
				}
			}
			else
				std::cout << e2 << std::endl;
		}
	}
	else
		std::cout << e1 << std::endl;
	char c;
	std::cin >> c;
	return 0;
}
