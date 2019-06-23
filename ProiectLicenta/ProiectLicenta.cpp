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
	size_t pos = 0, caseNr;
	Parser::init();

	unsigned int condCounter = 0, condLine = 0;
	int condCase;

	


	std::string strLine, str, typeStr, errStr;

	std::vector<std::vector<std::string>> cases;
	cases.reserve(100);
	std::vector<std::string> auxCase;

	auxCase = Parser::readFile("fisier.txt");
	cases.push_back(auxCase);

	for (caseNr = 0; caseNr < cases.size(); ++caseNr)
	{
		Parser::clear();
		std::vector<std::string>& currCase = cases[caseNr];
		for (int i = 0; i < currCase.size(); ++i)
		{
			strLine = currCase[i];
			lineInFile = i + 1;

			Parser::overflow = false;

			//Stergere spatii goale de la inceputul si finalul randului
			StringHelper::formatStr(strLine);
			if (strLine.empty())
				continue;


			if (sTRUE == Parser::isDeclarationLine(strLine) && condCounter == 0)
			{

				pos = strLine.find(' ');
				typeStr = strLine.substr(0, pos);

				pos = strLine.find_first_not_of(" ", pos);
				str = strLine.substr(pos);

				ERRORLOGCONTINUE(sSUCCESS == Parser::parseVarDecl(Parser::typeMap[typeStr], str, Parser::varMap),
					"parseVarInit error on row " + std::to_string(lineInFile));

			}

			else if (sTRUE == Parser::isAttributionLine(strLine) && condCounter == 0)
			{
				ERRORLOGCONTINUE(sSUCCESS == Parser::parseVarAttr(strLine, Parser::varMap),
					"parseVarAttr error on row " + std::to_string(lineInFile));

			}

			else if (sTRUE == Parser::isFunctionLine(strLine) && condCounter == 0)
			{
				ERRORLOGCONTINUE(sSUCCESS == Parser::parseVarFunction(strLine, Parser::varMap),
					"parseVarFunction error on row " + std::to_string(lineInFile));
			}

			else if (sTRUE == Parser::isConditionLine(strLine))
			{
				std::cout << "Line " + std::to_string(lineInFile) + " is condition line!\n";		
				if (condCounter == 0)
				{					
					std::pair<std::string, std::string> strPair;
					ERRORLOGCONTINUE(sSUCCESS == Parser::parseVarCondition(strLine, Parser::varMap, condCase, strPair),
						"parseVarFunction error on row " + std::to_string(lineInFile));
					if (ISPOSONLY == condCase)
					{
						std::cout << "Positive only!\n";
						currCase[i] = "";
					}
					else if (ISNEGONLY == condCase)
					{
						std::cout << "Negative only!\n";
						currCase[i] = "";
					}
					else if (ISBOTH == condCase)
					{
						std::vector<std::string> auxVect;
						for (int j = 0; j < currCase.size(); j++)
							auxVect.push_back(currCase[j]);
						cases.push_back(auxVect);
						currCase[i] = strPair.first;
						std::cout << strPair.first;
						cases[cases.size() - 1][i] = strPair.second;
					}
					condLine = i;
				}
				++condCounter;
				
			}

			else if (sTRUE == Parser::isClosedBracketLine(strLine))
			{
				ERRORLOGCONTINUE(condCounter > 0, "Close bracket on line " + std::to_string(lineInFile) +
					" doesn't have any correspondent open bracket");
				--condCounter;
				if (condCounter == 0)
				{
					if (ISPOSONLY == condCase)
					{
						currCase[i] = "";
					}
					else if (ISNEGONLY == condCase)
					{
						currCase[i] = "";
						for (int j = condLine + 1; j < i; ++j)
						{
							currCase[j] = "";
						}
					}
					else if (ISBOTH == condCase)
					{
						currCase[i] = "";
						cases[cases.size() - 1][i] = "";
						for (int j = condLine + 1; j < i; ++j)
						{
							cases[cases.size() - 1][j] = "";
						}
					}
					i = -1;
					Parser::clear();
				}
			}

			else if(condCounter == 0)
			{
				std::cout << "Nu e nici o linie!\n";
			}

		}
	}

	//for loop to see the range of all variables declared in the program
	for (auto it = Parser::typeMap.begin(); it != Parser::typeMap.end(); ++it)
	{
		std::cout << it->first << " " << it->second.min << " " << it->second.max << '\n';
	}
	//Print all the overflows found in the code
	SHOWOVERFLOWS();

	return sSUCCESS;

}
