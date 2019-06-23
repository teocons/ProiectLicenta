#pragma once
#ifndef StringHelper_H
#define StringHelper_H



#include <string>
#include <iostream>
#include <cctype>
#include <algorithm>

#include "ErrorLog.h"


const std::string EQUALSIGN = "==";
const std::string DIFFERENTSIGN = "!=";
const std::string LESSSIGN = "<";
const std::string LESSEQUALSIGN = "<=";
const std::string GREATERSIGN = ">";
const std::string GREATEREQUALSIGN = ">=";

namespace StringHelper
{

	size_t nrOfAppearance(const std::string& str, const std::string& findStr)
	{
		size_t nrApp = 0;
		for(char c : findStr)
		{
			for(char d : str)
			{
				if(c == d)
				{
					++nrApp;
				}
			}
		}
		return nrApp;
	}

	std::string addBrackets(const std::string& str)
	{
		std::string newStr = "[" + str + "]";
		return newStr;
	}

	std::string addBrackets(size_t x)
	{
		std::string newStr = "[" + std::to_string(x) + "]";
		return newStr;
	}



	void removeFromBeginOfStr(std::string& str, const std::string& rmvString)
	{
		size_t pos = str.find_first_not_of(rmvString);
	
		if(pos != std::string::npos)
		{
			str = str.substr(pos);
		}
		else
		{
			str = "";
		}
	}
	
	void removeFromEndOfStr(std::string& str, const std::string& rmvString)
	{
		size_t pos = str.find_last_not_of(rmvString);
	
		if(pos != std::string::npos)
		{
			str = str.substr(0, pos + 1);
		}
		else
		{
			str = "";
		}
	}
	
	void removeFromBothEndsOfStr(std::string& str, const std::string& rmvString)
	{
		removeFromBeginOfStr(str, rmvString);
		removeFromEndOfStr(str, rmvString);
	}
	
	
	//Remove every occurence of every one of the chars in rmvString from str
	void removeFromStr(std::string& str, const std::string& rmvString)
	{
		for(char c : rmvString)
		{
			str.erase(std::remove(str.begin(), str.end(), c), str.end());
		}
	}

	void removeCommentFromStr(std::string& str)
	{
		size_t pos = str.find("//");
		if (pos != std::string::npos)
		{
			str = str.substr(0, pos);
		}
	}

	void formatStr(std::string& str)
	{
		removeFromBeginOfStr(str, " ;\r\n");
		removeCommentFromStr(str);
		removeFromEndOfStr(str, " \r\n");
	}


	
	bool isEmptyLine(std::string str)
	{
		StringHelper::removeFromStr(str, " ;\r\n");
		return str.empty();
	}
	
	bool isNumber(const std::string& str)
	{
		if (!isdigit(str[0]) && str[0] != '-')
		{
			return false;
		}
		for(int i = 1; i < str.size(); ++i)
		{
			if(!isdigit(str[i]))
			{
				return false;
			}
		}		
		return true;		
	}
	
	bool isOperator(char c)
	{
		if(c == '+' or c == '-' or c == '*' or c == '/')
		{
			return true;
		}
		return false;
	}

	bool isCompOperator(const std::string& str)
	{
		return (str == EQUALSIGN || str == DIFFERENTSIGN || str == LESSSIGN || str == LESSEQUALSIGN
			|| str == GREATERSIGN || str == GREATEREQUALSIGN);
	}
	
	std::string namePartOfArray(const std::string& oldStr)
	{
		size_t posLeftBracket;
		if(oldStr[oldStr.size() - 1] != ']')
		{
			return oldStr;
		}

		posLeftBracket = oldStr.rfind('[');
		if(posLeftBracket == std::string::npos)
		{
			return oldStr;
		}

		if(posLeftBracket == oldStr.size() - 3)
		{
			if(oldStr[posLeftBracket + 1] == '0')
			{
				return oldStr;
			}
		}

		for(size_t i = posLeftBracket + 1; i < oldStr.size() - 1; ++i)
		{
			if(!isdigit(oldStr[i]))
			{
				return oldStr;
			}
		}

		return oldStr.substr(0, posLeftBracket);

	}

	size_t sizeOfArray(const std::string& str)
	{
		size_t posLeftBracket;
		if(str[str.size() - 1] != ']')
		{
			return 0;
		}

		posLeftBracket = str.rfind('[');
		if(posLeftBracket == std::string::npos)
		{
			return 0;
		}

		if(str.size() - posLeftBracket == 2)
		{
			return 0;
		}

		std::string numStr = str.substr(posLeftBracket+1, str.size() - posLeftBracket - 2);

		return static_cast<size_t> (std::stoul(numStr));

	}

	std::string rangeToAttributionLine(const std::string& varName, long long minVal, long long maxVal)
	{
		std::string retStr;
		retStr = varName + "=[" + std::to_string(minVal) + "," + std::to_string(maxVal) + "];";
		return retStr;
	}


	//Verify if the argument string can be a viabile variable name in conformity with C standards
	bool isGoodVarName(const std::string& strVar)
	{
		if(strVar.size() == 0)
		{
			return sFALSE;
		}

		const std::string& strRest = namePartOfArray(strVar);

		char c = strRest[0];
		if (!((c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z') or c == '_'))
		{
			return sFALSE;
		}

		for(char c : strRest)
		{
			if (!((c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z') or (c >= '1' and c <= '9') or c == '_'))
			{
				return sFALSE;
			}
		}

		return sTRUE;
	}

	bool isGoodArrayName(const std::string& strVar)
	{
		return (strVar[strVar.size() - 1] == ']');
	}

}


#endif
