#ifndef ErrorLog_H
#define ErrorLog_H

#include <string>
#include <vector>
#include <iostream>

#define sERROR -1
#define sSUCCESS 0
#define sFALSE false
#define sTRUE true

std::vector<std::string> overflowLines;
size_t lineInFile;

#define ERRORLOG(boolean, str) 																						\
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

#define ERRORLOGCONTINUE(boolean, str	) 																						\
	if(!(boolean)) 																											\
	{				  																										\
		std::cout << "ERROR in file  " << __FILE__ << " at line " << __LINE__ << " with message: " << str << "!\n"; 		\
		continue;																										\
	}

#define WARNINGLOG(boolean, str, error) 																					\
	if(!(boolean)) 																											\
	{				  																										\
		std::cout << "WARNING in file  " << __FILE__ << " at line " << __LINE__ << " with message: " << str << "!\n"; 	\																									\
	}


void OVERFLOWLOG(const std::string& str)
{

	std::string auxStr = "Line " + std::to_string(lineInFile) + ":";
	auxStr += str;
	auxStr += ".\n";
	overflowLines.push_back(auxStr);
}

void SHOWOVERFLOWS()
{
	if (overflowLines.size() == 0)
	{
		std::cout << "No overflows found!\n";
	}

	else
	{
		std::cout << "Found the following overflows:\n";
		for (const std::string& str : overflowLines)
		{
			std::cout << str << '\n';
		}
	}
}


#endif


