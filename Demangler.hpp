//Copyright 2020 Iris Technologies, All Rights Reserved
#pragma once
#include <string>

using namespace std;

namespace ProjectIris
{
    namespace Demangler
    {
        //I know it's pretty simple, but i have other priorities, TODO
        __forceinline string Demangle(const string& Name) noexcept
        {
            string name(Name);
            if(name[0] == '_')
                name.erase(0, 1);
            for (size_t i = name.size() - 1; i >= 0; --i)
                if (name[i] < '0' || name[i] > '9')
                {
                    if (name[i] == '@')
                        name.erase(i, name.size() - i);
                    break;
                }
            return name;
        }
    }
}
