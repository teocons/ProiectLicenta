#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdio>

#include "pch.h"
#include "exprEval.h"
#include "StringHelper.h"
#include "Type.h"
#include "Parser.h"
#include "ErrorLog.h"


int main()
{
	int i = 0;
	size_t pos = 0;
	Parser::init();

	std::string strLine, str, typeStr, errStr;

	std::ifstream fin("fisier.txt");

	lineInFile = 0;
	while (std::getline(fin, strLine))
	{
		++i;
		++lineInFile;
		Parser::overflow = false;


		//Stergere spatii goale de la inceputul si finalul randului
		StringHelper::removeFromBeginOfStr(strLine, " ;\r\n");
		StringHelper::removeFromEndOfStr(strLine, " \r\n");
		if (strLine.empty())
			continue;


		if (sTRUE == Parser::isDeclarationLine(strLine))
		{

			pos = strLine.find(' ');
			typeStr = strLine.substr(0, pos);

			pos = strLine.find_first_not_of(" ", pos);
			str = strLine.substr(pos);

			ERRORLOGCONTINUE(sSUCCESS == Parser::parseVarDecl(Parser::typeMap[typeStr], str, Parser::varMap),
				"parseVarInit error on row " + std::to_string(i));

		}

		else if (sTRUE == Parser::isAttributionLine(strLine))
		{

			ERRORLOGCONTINUE(sSUCCESS == Parser::parseVarAttr(strLine, Parser::varMap),
				"parseVarAttr error on row " + std::to_string(i));

		}

	}

	//for loop to see the range of all variables declared in the program
	for (auto it = Parser::varMap.begin(); it != Parser::varMap.end(); ++it)
	{
		std::cout << it->first << " " << it->second.min << " " << it->second.max << '\n';
	}
	//Print all the overflows found in the code
	SHOWOVERFLOWS();

	return sSUCCESS;

}
