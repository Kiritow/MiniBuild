#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>
using namespace std;
/// Declaration
/// Set 'skiplevel' to <1 to stop skipping.
/// Set 'maxlevel' to <1 to ignore max level.
void FindFileRev(const string& dirname,
                 const int skiplevel,const int maxlevel,
                 const function<void(const string&)>& func);
vector<string> FindSourceFile(const string& BasicDir,const vector<string>& SourceSuffix);
bool EndWith(const string& Source,const string& ToFind);
string ReplaceEnd(const string& Source,const string& ToFind,const string& ToReplace);
bool IsExist(const string& Filename);
struct edtime
{
    int y,m,d;
    int hh,mm,ss;
};
edtime GetEditTime(const string& Filename);

struct compileinfo
{
    const string suffix;
    const string bin;
    const string flag;

    compileinfo(const string& Suffix,const string& Bin,const string& Flag) :
        suffix(Suffix), bin(Bin), flag(Flag) {}
};

const vector<compileinfo>& CINFO
{
    compileinfo(".c","gcc","-Wall -s -O2"),
    compileinfo(".cpp","g++","-Wall -fexceptions -std=c++14 -s -O2")
};
const string SEARCH_DIR = ".";

const string linker = "g++";
const string ldflag = "-lws2_32 -fno-common ";
const string program_name = "program_name.exe";

const int SKIP_DIR = 0;

void CMDBuild()
{
    /// Find Source File
    vector<vector<string>> sourcelist(CINFO.size());
    int sourcecnt=0;
    auto func=[&](const string& str)
    {
        for(size_t i=0;i<CINFO.size();++i)
        {
            if(EndWith(str,CINFO[i].suffix))
            {
                sourcelist[i].push_back(str);
                ++sourcecnt;
                break;
            }
        }
    };
    printf("Searching Source Files...\n");
    FindFileRev(SEARCH_DIR,0,0,func);
    if(sourcecnt<1)
    {
        printf("No Source File Found.\n");
        return ;
    }

    /// Compile
    vector<string> objvec;
    for(size_t i=0;i<CINFO.size();i++)
    {
        for(size_t j=0;j<sourcelist[i].size();j++)
        {
            string obj=ReplaceEnd(sourcelist[i][j],CINFO[i].suffix,".o");
            string command = CINFO[i].bin + " " + CINFO[i].flag + " -c " + sourcelist[i][j] + " -o " + obj;
            system(command.c_str());
            objvec.push_back(obj);
        }
    }
    /// Link
    string command = linker + " -o " + program_name ;
    for(auto& objref : objvec)
    {
        command += " "s + objref;
    }
    command += " "s + ldflag;
    system(command.c_str());
}

/// Main Function
int main()
{
    CMDBuild();
    return 0;
}



/// Implement
string ReplaceEnd(const string& Source,const string& ToFind,const string& ToReplace)
{
    return Source.substr(0,Source.size()-ToFind.size()).append(ToReplace);
};
bool EndWith(const string& Source,const string& ToFind)
{
    return (Source.substr(Source.size()-ToFind.size())==ToFind);
};
bool IsExist(const string& Filename)
{
    return false;
}


#if defined(_MSC_VER) || defined(_WIN32) /// VS or Windows
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <functional>
void _FindFileRev(const std::string& dirname,
                  const int skiplevel,const int maxlevel,int nowlevel,
                  const std::function<void(const std::string&)>& func)
{
    std::string patternString=dirname+"*";

    WIN32_FIND_DATA fnd;
    HANDLE hand=FindFirstFile(patternString.c_str(),&fnd);
    if(hand!=INVALID_HANDLE_VALUE)
    {
        do
        {
            std::string fullname=dirname+fnd.cFileName;
            if(fnd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if(maxlevel>0 && nowlevel<maxlevel)
                {
                    fullname.append("\\");
                    _FindFileRev(fullname,skiplevel,maxlevel,nowlevel+1,func);
                }
            }
            else
            {
                if(nowlevel>skiplevel) func(fullname);
            }
        }
        while(FindNextFile(hand,&fnd));
        FindClose(hand);
    }
}
void FindFileRev(const std::string& dirname,
                 const int skiplevel,const int maxlevel,
                 const std::function<void(const std::string&)>& func)
{
    if(dirname[dirname.size()-1]!='\\')
    {
        string dirnamex=dirname+"\\";
        _FindFileRev(dirnamex,skiplevel,maxlevel,1,func);
    }
    else
    {
        _FindFileRev(dirname,skiplevel,maxlevel,1,func);
    }
}
#else /// Linux-like
#include <dirent.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

void _FindFileRev(const std::string& dirname,
                 const int skiplevel,const int maxlevel,int nowlevel,
                 const std::function<void(const std::string&)>& func)
{
    DIR* Dir = NULL;
    struct dirent* file = NULL;

    if ((Dir = opendir(dirname.c_str())) == NULL)
    {
        return ;
    }
    while ((file = readdir(Dir)) != nullptr)
    {
        if (file->d_type == DT_REG)
        {
            if(nowlevel>skiplevel) func(dirname + file->d_name);
        }
        else if (file->d_type == DT_DIR && strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0)
        {
            if(maxlevel>0 && nowlevel<maxlevel)
            {
                _FindFileRev(dirname + file->d_name,skiplevel,maxlevel,nowlevel+1,func);
            }
        }
    }
    closedir(Dir);
}

void FindFileRev(const std::string& dirname,
                 const int skiplevel,const int maxlevel,
                 const std::function<void(const std::string&)>& func)
{
    if (dirname[dirname.size()-1] != '/')
    {
        string curDir=dirname+"/";
        _FindFileRev(curDir,skiplevel,maxlevel,1,func);
    }
    else
    {
        _FindFileRev(dirname,skiplevel,maxlevel,1,func);
    }
}
#endif
