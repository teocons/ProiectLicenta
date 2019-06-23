#ifndef ErrorLog_H
#define ErrorLog_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>

#define sERROR -1
#define sSUCCESS 0
#define sFALSE false
#define sTRUE true

const std::string outputFileName = "Overflow.txt";

std::set<std::string> errorSet;

std::vector<std::string> overflowLines;
size_t lineInFile;

#define ERRORLOG(boolean, str) 																								\
	if(!(boolean)) 																											\
	{				  																										\
		std::cout << "ERROR in file  " << __FILE__ << " at line " << __LINE__ << " with message: " << str << "!\n"; 		\																										\
	}

#define ERRORLOGRETURN(boolean, str, error) 																						\
	if(!(boolean)) 																											\
	{				  																										\
		std::cout << "ERROR in file  " << __FILE__ << " at line " << __LINE__ << " with message: " << str << "!\n"; 		\
		return error;																										\
	}

#define ERRORLOGCONTINUE(boolean, str) 																						\
	if(!(boolean)) 																											\
	{				  																										\
		std::cout << "ERROR in file  " << __FILE__ << " at line " << __LINE__ << " with message: " << str << "!\n"; 		\
		continue;																											\
	}

#define WARNINGLOG(boolean, str, error) 																					\
	if(!(boolean)) 																											\
	{				  																										\
		std::cout << "WARNING in file  " << __FILE__ << " at line " << __LINE__ << " with message: " << str << "!\n";		\																									\
	}


void OVERFLOWLOG(const std::string& str)
{

	std::string auxStr = "Line " + std::to_string(lineInFile) + ":";
	auxStr += str;
	auxStr += ".\n";
	auto it = errorSet.find(auxStr);
	if (it == errorSet.end())
	{
		errorSet.insert(auxStr);
		overflowLines.push_back(auxStr);
	}


}

void SHOWOVERFLOWS()
{

	std::ofstream fout(outputFileName);

	if (overflowLines.size() == 0)
	{
		fout << "No overflows found!\n";
	}

	else
	{
		fout << "Found the following overflows:\n\n";
		for (const std::string& str : overflowLines)
		{
			fout << str << '\n';
		}
	}
}


#endif


