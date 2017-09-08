/// mfgenerator.hpp
/// Based on HCTECH/FileSystemUtil. (By Kiritow). Rewritten by Kiritow.
/// Under MIT License. Part of MiniBuild Project.
/// This file provides some file operating functions.

#include <functional>
#include <string>

/// Implement
#if defined(_MSC_VER) || defined(_WIN32) /// VS or Windows
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
void FindFileRev(const std::string& dirname,const std::function<void(const std::string&)>& func)
{
    std::string patternString;
    if(dirname[dirname.size()-1]!='\\')
    {
        patternString=dirname+"\\*";
    }
    else
    {
        patternString=dirname+"*";
    }

    WIN32_FIND_DATA fnd;
    HANDLE hand=FindFirstFile(patternString.c_str(),&fnd);
    if(hand!=INVALID_HANDLE_VALUE)
    {
        do
        {
            std::string fullname=dirname+fnd.cFileName;
            if(fnd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                fullname.append("\\");
                FindFileRev(fullname,func);
            }
            else
            {
                func(fullname);
            }
        }
        while(FindNextFile(hand,&fnd));
        FindClose(hand);
    }
}
#else /// Linux-like
#include <dirent.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

void FindFileRev(const std::string& dirname,const std::function<void(const std::string&)>& func)
{
    DIR* Dir = NULL;
    struct dirent* file = NULL;
    std::string curDir;
    if (dirname[dirname.size()-1] != '/')
    {
        curDir=dirname+"/";
    }
    else
    {
        curDir=dirname;
    }

    if ((Dir = opendir(curDir.c_str())) == NULL)
    {
        return ;
    }
    while ((file = readdir(Dir)) != nullptr)
    {
        if (file->d_type == DT_REG)
        {
            func(curDir + file->d_name);
        }
        else if (file->d_type == DT_DIR && strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0)
        {
            FindFileRev(curDir + file->d_name,func);
        }
    }
    closedir(Dir);
}
#endif
