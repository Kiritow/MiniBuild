/// mfgenerator.cpp
/// Under MIT License. Part of MiniBuild Project.
/// You can run this code to generate simple makefile.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

#include "mfgenerator.hpp"

using namespace std;

const string suffix_c=".c";
const string suffix_cpp=".cpp";
const string c_flags="-Wall -s -O2";
const string cpp_flags="-Wall -std=c++11 -s -O2";
const string ld_flags="";
const string ld_libs="";
const string out_file="prog_name";

const vector<string> excludevec
{"mfgenerator"};

int main()
{
	printf("mfgenerator: Scanning...\n");
	vector<string> vec_c;
	vector<string> vec_cpp;
	FindFileRev(".",[&](const string& s)
	{
		size_t x=s.rfind(suffix_c);
		if(x!=string::npos&&x+suffix_c.size()==s.size())
		{
			string ns=s;
			ns.replace(x,suffix_c.size(),"");
			vec_c.push_back(ns);
		}
		else
		{
			x=s.rfind(suffix_cpp);
			if(x!=string::npos&&x+suffix_cpp.size()==s.size())
			{
				string ns=s;
				ns.replace(x,suffix_cpp.size(),"");
				vec_cpp.push_back(ns);
			}
		}
	});
	
	printf("mfgenerator: Excluding...\n");
	int cexi=excludevec.size();
	vec_c.erase(remove_if(vec_c.begin(),vec_c.end(),[&](const string& s)
	{
		for(int i=0;i<cexi;i++)
		{
			if(s.find(excludevec[i])!=string::npos)
			{
				return true;
			}
		}
		
		return false;
	}),vec_c.end());
	vec_cpp.erase(remove_if(vec_cpp.begin(),vec_cpp.end(),[&](const string& s)
	{
		for(int i=0;i<cexi;i++)
		{
			if(s.find(excludevec[i])!=string::npos)
			{
				return true;
			}
		}
		
		return false;
	}),vec_cpp.end());	
	
	printf("mfgenerator: Generating...\n");
	FILE* fp=fopen("makefile","w");
	fprintf(fp,"CFLAGS= %s\n",c_flags.c_str());
	fprintf(fp,"CXXFLAGS= %s\n",cpp_flags.c_str());
	fprintf(fp,"LDFLAGS= %s\n",ld_flags.c_str());
	fprintf(fp,"LDLIBS= %s\n",ld_libs.c_str());
	fprintf(fp,"\n");
	fprintf(fp,"all: do_build\n\ndo_build: do_compile_c do_compile_cpp do_link\n\n");
	int ci=vec_c.size();
	for(int i=0;i<ci;i++)
	{
		auto& vec=vec_c;
		fprintf(fp,"c%d: %s.c\n\t$(CC) $(CFLAGS) -c %s.c -o %s.o\n",i,vec[i].c_str(),vec[i].c_str(),vec[i].c_str());
	}
	int cppi=vec_cpp.size();
	for(int i=0;i<cppi;i++)
	{
		auto& vec=vec_cpp;
		fprintf(fp,"cp%d: %s.cpp\n\t$(CXX) $(CXXFLAGS) -c %s.cpp -o %s.o\n",i,vec[i].c_str(),vec[i].c_str(),vec[i].c_str());
	}
	fprintf(fp,"do_compile_c: ");
	for(int i=0;i<ci;i++) fprintf(fp,"c%d ",i);
	fprintf(fp,"\n\n");
	
	fprintf(fp,"do_compile_cpp: ");
	for(int i=0;i<cppi;i++) fprintf(fp,"cp%d ",i);
	fprintf(fp,"\n\n");
	
	fprintf(fp,"do_link: \n\t$(CXX) $(LDFLAGS) -o %s ",out_file.c_str());
	for(int i=0;i<ci;i++) fprintf(fp,"%s.o ",vec_c[i].c_str());
	for(int i=0;i<cppi;i++) fprintf(fp,"%s.o ",vec_cpp[i].c_str());
	fprintf(fp," $(LDLIBS)\n");
	
	fprintf(fp,"clean: \n\trm ");
	for(int i=0;i<ci;i++) fprintf(fp,"%s.o ",vec_c[i].c_str());
	for(int i=0;i<cppi;i++) fprintf(fp,"%s.o ",vec_cpp[i].c_str());
	fprintf(fp,"\n");
	
	fclose(fp);
	printf("mfgenerator: Done.\n");
	return 0;
}
