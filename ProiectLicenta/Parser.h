#pragma once
#ifndef Parser_H
#define Parser_H

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <string>
#include <utility>

#include "exprEval.h"
#include "StringHelper.h"
#include "Type.h"
#include "ErrorLog.h"

namespace Parser
{
	const std::string READKEY = "citire";
	const std::string WRITEKEY = "afisare";
	const std::string IFCONDITION = "if";
	std::unordered_set<std::string> keyTypes;




	std::unordered_map<std::string, DataType> typeMap;
	std::unordered_map<std::string, Var> varMap;
	std::unordered_map<std::string, Array> arrayMap;
	
	bool overflow = false;

	void clear()
	{
		varMap.clear();
		arrayMap.clear();
	}

	std::vector<std::string> readFile(const std::string& fileName)
	{
		std::vector<std::string> lines;
		std::string strLine;
		std::ifstream fin(fileName);

		while (std::getline(fin, strLine))
		{
			lines.push_back(strLine);
		}
		return lines;
	}


	void readDataTypes()
	{
		std::vector<std::string> lines = readFile("configDataType.txt");
		DataType auxType;
		for (size_t i = 0; i < lines.size(); ++i)
		{
			std::string line = lines[i];
			StringHelper::formatStr(line);
			if (line.empty())
				continue;
			std::string nrStr;
			int pos = 0, lastpos = 0;
			pos = line.find_first_not_of(" ");
			lastpos = pos;
			pos = line.find_first_of(" ", lastpos);
			auxType.name = line.substr(lastpos, pos - lastpos);
			lastpos = pos;
			pos = line.find_first_not_of(" ", lastpos);
			lastpos = pos;
			pos = line.find_first_of(" ", lastpos);
			nrStr = line.substr(lastpos, pos - lastpos);
			auxType.min = stoll(nrStr);
			lastpos = pos;
			pos = line.find_first_not_of(" ", lastpos);
			lastpos = pos;
			pos = line.find_first_of(" ", lastpos);
			if (pos == std::string::npos)
			{
				pos = line.size();
			}
			nrStr = line.substr(lastpos, pos - lastpos);
			auxType.max = stoll(nrStr);
			typeMap[auxType.name] = auxType;
		}
	}
	
	void init()
	{
		typeMap.clear();
		varMap.clear();
		arrayMap.clear();
		keyTypes.clear();
		readDataTypes();

	}
	
	int findNumValues(const std::string& str, std::vector<Var>& retVect)
	{
		size_t pos = 0, lastpos = 0;
		size_t nrOfCommas = 0;
		std::string auxStr;
		Var evl;

		pos = str.find_first_not_of(" ,");

		auxStr = str.substr(0, pos);

		nrOfCommas = StringHelper::nrOfAppearance(auxStr, ",");

		ERRORLOGRETURN(nrOfCommas < 2, "There are " + std::to_string(nrOfCommas) + " without anything between them", sERROR);

		if(pos == std::string::npos)
		{

			ERRORLOGRETURN(nrOfCommas == 0, "Have to be something after the final comma", sERROR);
			return sSUCCESS;
		}

		lastpos = pos;

		pos = str.find_first_of(",;", pos);
		if(pos == std::string::npos)
		{
			pos = str.size();
		}
		else
		{
			auxStr = str.substr(pos);
			if(sERROR == findNumValues(auxStr, retVect))
			{
				return sERROR;
			}
		}

		auxStr = str.substr(lastpos, pos - lastpos);

		ERRORLOGRETURN(sSUCCESS == Expression::evaluate(auxStr, varMap, evl), "Expression::evaluation return error", sERROR);
		retVect.push_back(evl);

		return sSUCCESS;

	}

	
	//Store initialized and maybe declared variables in a map of variables
	int parseVarDecl(const DataType& varType, const std::string& str, std::unordered_map<std::string, Var>& varMap)
	{

		size_t pos = 0, lastpos = 0;
		std::string varName, auxStr, strValue;
		do
		{

			pos = str.find_first_not_of(" ,", pos);
			ERRORLOGRETURN(pos != std::string::npos, "String str is empty", sERROR);
				
			lastpos = pos;
				
			pos = str.find_first_of(" =,;", pos);
			ERRORLOGRETURN(pos != std::string::npos, "No line termination", sERROR);
				
			varName = str.substr(lastpos, pos - lastpos);

			ERRORLOGRETURN(sTRUE == StringHelper::isGoodVarName(varName), "The variable name: " + varName + " is not acceptable!", sERROR);

			pos = str.find_first_not_of(' ', pos);

			if(StringHelper::isGoodArrayName(varName))
			{
				ERRORLOGRETURN(arrayMap.find(varName) == arrayMap.end(), "VariabilE " + varName + " is already declared", sERROR);

				const std::string& arrayName = StringHelper::namePartOfArray(varName);
				const size_t arraySize = StringHelper::sizeOfArray(varName);

				if(arraySize == 0)
				{

					ERRORLOGRETURN(varType == typeMap["char"], 
						"Can't initialize array with name " + varName + 
						" without size if it has another type than char", sERROR);

					ERRORLOGRETURN(str[pos] == '=',
						"Char array without size should be initialized", sERROR);
					++pos;
					pos = str.find_first_not_of(" ", pos);

					ERRORLOGRETURN(str[pos] == '"',
						"You need to initialize parameter: " + varName + " with a char*", sERROR);

					++pos;
					lastpos = pos;
					pos = str.find('"', pos);
					ERRORLOGRETURN(pos != std::string::npos, "You forgot to end the string with \"", sERROR);

					strValue = str.substr(lastpos, pos - lastpos);

					Array auxArray = {strValue.size(), varName};
					Var auxVar;
					auxVar.limMin = typeMap["char"].min;
					auxVar.limMax = typeMap["char"].max;

					arrayMap[varName] = auxArray;

					long long val;

					for(size_t i = 0; i < strValue.size(); ++i)
					{
						val = static_cast<long long>(strValue[i]);
						auxVar.min = val;
						auxVar.max = val;
						auxVar.name = arrayName + StringHelper::addBrackets(i);
						varMap[auxVar.name] = auxVar;
					}

				}
				else
				{
					Array auxArray = {arraySize, varName};
					arrayMap[varName] = auxArray;

					if(str[pos] == '=')
					{
						++pos;
						pos = str.find_first_not_of(" ", pos);

						if(varType == typeMap["char"])
						{
							ERRORLOGRETURN(str[pos] == '"', "You need to initialize parameter: " + varName + " with a char*", sERROR);

							++pos;
							lastpos = pos;
							pos = str.find('"', pos);

							ERRORLOGRETURN(pos != std::string::npos, "You forgot to end the string with \"", sERROR);

							strValue = str.substr(lastpos, pos - lastpos);

							Var auxVar;
							auxVar.limMin = typeMap["char"].min;
							auxVar.limMax = typeMap["char"].max;

							long long val;

							if(strValue.size() > arraySize)
							{
								OVERFLOWLOG("Size of variable " + varName + " is smaller than size of string with which it should be initialized");
								for(size_t i = 0; i < arraySize; ++i)
								{
									val = static_cast<long long>(strValue[i]);
									auxVar.min = val;
									auxVar.max = val;
									auxVar.name = arrayName + StringHelper::addBrackets(i);
									varMap[auxVar.name] = auxVar;
								}
							}

							else
							{
								for(size_t i = 0; i < strValue.size(); ++i)
								{
									val = static_cast<long long>(strValue[i]);
									auxVar.min = val;
									auxVar.max = val;
									auxVar.name = arrayName + StringHelper::addBrackets(i);
									varMap[auxVar.name] = auxVar;
								}

								auxVar.min = typeMap["char"].min;
								auxVar.max = typeMap["char"].max;

								for(size_t i = strValue.size(); i < arraySize; ++i)
								{
									auxVar.name = arrayName + StringHelper::addBrackets(i);
									varMap[auxVar.name] = auxVar;
								}
							}
						}
						else
						{
							ERRORLOGRETURN(str[pos] == '{', "You need to introduce { after the equal sign", sERROR);

							++pos;
							lastpos = pos;
							pos = str.find('}', pos);

							ERRORLOGRETURN(pos != std::string::npos, "You forgot to end the string with }", sERROR);

							strValue = str.substr(lastpos, pos - lastpos);



							std::vector<Var> values;

							ERRORLOGRETURN(sSUCCESS == findNumValues(strValue, values), "Couldn't parse numbers from string " + strValue, sERROR);

							values = vecReversal(values);

							Var auxVar;
							auxVar.limMin = varType.min;
							auxVar.limMax = varType.max;

							if(values.size() > arraySize)
							{
								OVERFLOWLOG("Size of variable " + varName + " is smaller than size of array with which it should be initialized");
								for(size_t i = 0; i < arraySize; ++i)
								{
									auxVar.min = values[i].min;
									auxVar.max = values[i].max;
									auxVar.name = arrayName + StringHelper::addBrackets(i);
									varMap[auxVar.name] = auxVar;
								}
							}
							else
							{
								for(size_t i = 0; i < values.size(); ++i)
								{
									auxVar.min = values[i].min;
									auxVar.max = values[i].max;
									auxVar.name = arrayName + StringHelper::addBrackets(i);
									varMap[auxVar.name] = { varType.min, varType.max, varType.min, varType.max, auxVar.name };
									if (sERROR == attribute(auxVar, varMap[auxVar.name]))
									{
										OVERFLOWLOG("Value overflow when attributing result to variable " + auxVar.name);
									}
								}

								auxVar.min = varType.min;
								auxVar.max = varType.max;


								for(size_t i = values.size(); i < arraySize; ++i)
								{
									auxVar.name = arrayName + StringHelper::addBrackets(i);
									varMap[auxVar.name] = auxVar;
								}

							}

						}
					}
				}

			}
			else
			{
				if(str[pos] == ';' or str[pos] == ',')
				{

					ERRORLOGRETURN(varMap.find(varName) == varMap.end(), "Variable " + varName + " already initialized", sERROR);
					varMap[varName] = {varType.min, varType.max, varType.min, varType.max, varName};
				}
				else if(str[pos] == '=')
				{
					ERRORLOGRETURN(varMap.find(varName) == varMap.end(), "Variable " + varName + " already initialized", sERROR);
					varMap[varName] = { varType.min, varType.max, varType.min, varType.max, varName };
					Var evl;
					evl.name = varName;
					++pos;
					lastpos = pos;

					pos = str.find_first_of(",;=", pos);
					auxStr = str.substr(lastpos, pos - lastpos);

					if(str[pos] == ';' or str[pos] == ',')
					{
						ERRORLOGRETURN(sSUCCESS == Expression::evaluate(auxStr, varMap, evl), "Can't evaluate expression: " + auxStr, sERROR);

						if (sERROR == attribute(evl, varMap[varName]))
						{
							OVERFLOWLOG("Value overflow when attributing result to variable " + varName);
						}
					}
				}
				else
				{
					ERRORLOGRETURN(false, "Can't parsing variable " + varName, sERROR);
				}
			}
		} while(pos < str.size() and str[pos] == ',');
		
	
		return sSUCCESS;
	
	}
	

	//Modify variable value in the map of variables
	int parseVarAttr(const std::string& str, std::unordered_map<std::string, Var>& varMap)
	{
		size_t pos = 0, lastpos = 0;
		std::string varName, auxStr;
		Var currentVar, newVar;

		pos = str.find_first_of(" =");

		ERRORLOGRETURN(pos != std::string::npos,"Can't parse " + str + " because it doesn't have any operator", sERROR);

		varName = str.substr(lastpos,pos);

		ERRORLOGRETURN(varMap.find(varName) != varMap.end(), "The variable " + varName + " hasn't been initialized", sERROR);

		currentVar = varMap[varName];

		lastpos = pos;
		pos = str.find_first_not_of(" ", pos);

		ERRORLOGRETURN(str[pos] == '=', "After variable name there isn't an = sign", sERROR);

		++pos;
		pos = str.find_first_not_of(" ", pos);
		lastpos = pos;
		pos = str.find_first_of(";", pos);

		ERRORLOGRETURN(pos != std::string::npos, "Line doesn't end correctly", sERROR);

		auxStr = str.substr(lastpos, pos);
		StringHelper::removeFromEndOfStr(auxStr, " ;");

		ERRORLOGRETURN(sSUCCESS == Expression::evaluate(auxStr, varMap, newVar), "Can't evaluate expression " + auxStr, sERROR);

		if(sERROR == attribute(newVar, currentVar))
		{
			OVERFLOWLOG("Value overflow when attributing result to variable " + currentVar.name);
		}

		varMap[currentVar.name] = currentVar;

		return sSUCCESS;
	}

	int parseVarFunction(const std::string& strLine, std::unordered_map<std::string, Var>& varMap)
	{
		size_t pos = 0, lastpos = 0;
		std::string functionStr, varName;
		Var auxVar;

		pos = strLine.find_first_of(" ");
		functionStr = strLine.substr(0, pos);
		lastpos = pos;
		pos = strLine.find_first_not_of(" ", lastpos);
		ERRORLOGRETURN(pos != std::string::npos, "Can't parse " + strLine + " because it doesn't have any variable", sERROR);
		lastpos = pos;
		pos = strLine.find_first_of(" ;", lastpos);
		ERRORLOGRETURN(pos != std::string::npos, "Can't parse " + strLine + " because it doesn't end correctly", sERROR);
		varName = strLine.substr(lastpos, pos - lastpos);
		ERRORLOGRETURN(varMap.find(varName) != varMap.end(), "The variable " + varName + " hasn't been initialized", sERROR);
		auxVar = varMap[varName];
		pos = strLine.find_first_not_of(" ", pos);
		ERRORLOGRETURN(pos != std::string::npos, "Can't parse " + strLine + " because it doesn't end correctly", sERROR);
		ERRORLOGRETURN(strLine[pos] == ';', "Can't parse " + strLine + " because it doesn't end correctly", sERROR);
		if (functionStr == READKEY)
		{
			auxVar.min = auxVar.limMin;
			auxVar.max = auxVar.limMax;
			varMap[varName] = auxVar;
		}
		return sSUCCESS;
	}

	int parseVarCondition(const std::string& strLine, std::unordered_map<std::string, Var>& varMap, int& condCase, std::pair<std::string, std::string>& strPair)
	{
		size_t pos = 0, lastpos = 0;
		std::string varName, opStr, nrStr;
		Var var;
		std::pair<Var, Var> branchVars;
		long long number;

		Var auxVar;

		pos = strLine.find_first_of(" (");
		lastpos = pos;
		pos = strLine.find_first_not_of(" ", lastpos);
		ERRORLOGRETURN(pos != std::string::npos, "Can't parse " + strLine, sERROR);
		ERRORLOGRETURN(strLine[pos] == '(' , "Can't parse " + strLine + "because it doesn't have ( bracket!", sERROR);
		lastpos = pos + 1;
		pos = strLine.find_first_not_of(" ", lastpos);
		ERRORLOGRETURN(pos != std::string::npos, "Can't parse " + strLine, sERROR);
		lastpos = pos;
		pos = strLine.find_first_of(" ><!=", lastpos);
		ERRORLOGRETURN(pos != std::string::npos, "Can't parse " + strLine + "because it doesn't have a correct operator", sERROR);
		varName = strLine.substr(lastpos, pos - lastpos);
		ERRORLOGRETURN(varMap.find(varName) != varMap.end(), "The variable " + varName + " hasn't been initialized", sERROR);
		var = varMap[varName];
		lastpos = pos;
		pos = strLine.find_first_not_of(" ", lastpos);
		ERRORLOGRETURN(pos != std::string::npos, "Can't parse " + strLine, sERROR);
		lastpos = pos;
		pos = strLine.find_first_of(" 123456789", lastpos);
		ERRORLOGRETURN(pos != std::string::npos, "Can't parse " + strLine, sERROR);
		opStr = strLine.substr(lastpos, pos - lastpos);
		ERRORLOGRETURN(StringHelper::isCompOperator(opStr), "Can't parse " + strLine + " because operator is not good", sERROR);
		lastpos = pos;
		pos = strLine.find_first_not_of(" ", lastpos);
		ERRORLOGRETURN(pos != std::string::npos, "Can't parse " + strLine, sERROR);
		lastpos = pos;
		pos = strLine.find_first_not_of("0123456789", lastpos);
		ERRORLOGRETURN(pos != std::string::npos, "Can't parse " + strLine, sERROR);
		nrStr = strLine.substr(lastpos, pos - lastpos);
		ERRORLOGRETURN(StringHelper::isNumber(nrStr), "Can't parse " + strLine + " because " + nrStr + " isn't a number", sERROR);
		number = stoll(nrStr);

		branchVars = Expression::makeCompOp(var, number, opStr);
		condCase = Expression::whichCondCase(branchVars);
		strPair.first = StringHelper::rangeToAttributionLine(varName, branchVars.first.min, branchVars.first.max);
		strPair.second = StringHelper::rangeToAttributionLine(varName, branchVars.second.min, branchVars.second.max);

		std::cout << "a: " << var.min << " " << var.max << '\n';


		return sSUCCESS;
	}


	//Verify if the line of code is a declaration line (with or without initialization)
	bool isDeclarationLine(const std::string& strLine)
	{
		size_t pos = 0, lastpos = 0;
		std::string str;
	
		pos = strLine.find_first_of(" ");
		if(pos == std::string::npos)
		{
			return false;
		}
	
		str = strLine.substr(lastpos, pos - lastpos);
	
		return (typeMap.find(str) != typeMap.end());
	}
	
	//Verify if the line of code is a line in which we attribute a value to the element
	bool isAttributionLine(const std::string& strLine)
	{
		size_t pos = 0, lastpos = 0;
		std::string str;
	
		pos = strLine.find_first_of(" =");
		if(pos == std::string::npos)
		{
			return false;
		}

		lastpos = pos;
		pos = strLine.find_first_not_of(" ", lastpos);
		if (strLine[pos] != '=')
		{
			return false;
		}

		str = strLine.substr(0, lastpos);
	
		return (varMap.find(str) != varMap.end());
	}

	bool isFunctionLine(const std::string& strLine)
	{
		size_t pos = 0;
		std::string str;

		pos = strLine.find_first_of(" ");

		if (pos == std::string::npos)
		{
			return false;
		}

		str = strLine.substr(0, pos);

		return (str == READKEY || str == WRITEKEY);

	}

	bool isConditionLine(const std::string& strLine)
	{
		size_t pos = 0;
		std::string str;
		pos = strLine.find_first_of(" (");
		if (pos == std::string::npos)
		{
			return false;
		}

		str = strLine.substr(0, pos);

		return (str == IFCONDITION);
	}

	bool isClosedBracketLine(const std::string& strLine)
	{
		if (strLine.size() == 1 && strLine[0] == '}')
		{
			return sTRUE;
		}
		return sFALSE;
	}

}


#endif
