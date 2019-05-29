#pragma once
#ifndef Parser_H
#define Parser_H

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <string>

#include "exprEval.h"
#include "StringHelper.h"
#include "Type.h"
#include "ErrorLog.h"

namespace Parser
{
	
	std::unordered_set<std::string> keyTypes;
	std::unordered_map<std::string, Type> typeMap;
	std::unordered_map<std::string, Var> varMap;
	std::unordered_map<std::string, Array> arrayMap;
	
	bool overflow = false;
	
	void init()
	{
		Type auxType;
		auxType.min = -1000;
		auxType.max = 1000;
		auxType.name = "int";
		typeMap["int"] = auxType;
		keyTypes.emplace("int");
		auxType.name = "char";
		typeMap["char"] = auxType;
		keyTypes.emplace("char");

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
	int parseVarInit(const Type& varType, const std::string& str, std::unordered_map<std::string, Var>& varMap)
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
									varMap[auxVar.name] = auxVar;
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
					Var evl;
					evl.name = varName;
					++pos;
					lastpos = pos;

					pos = str.find_first_of(",;=", pos);
					auxStr = str.substr(lastpos, pos - lastpos);

					if(str[pos] == ';' or str[pos] == ',')
					{
						ERRORLOGRETURN(sSUCCESS == Expression::evaluate(auxStr, varMap, evl), "Can't evaluate expression: " + auxStr, sERROR);

						ERRORLOGRETURN(varMap.find(varName) == varMap.end(), "variable " + varName + " already initialized", sERROR);

						evl.limMin = varType.min;
						evl.limMax = varType.max;

						varMap[varName] = evl;


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

	bool isInitializationLine(const std::string& strLine)
	{
		size_t pos = 0, lastpos = 0;
		std::string str;
	
		pos = strLine.find_first_of(" ");
		if(pos == std::string::npos)
		{
			return false;
		}
	
		str = strLine.substr(lastpos, pos - lastpos);
	
		return (keyTypes.find(str) != keyTypes.end());
	}
	
	bool isAttributionLine(const std::string& strLine)
	{
		size_t pos = 0, lastpos = 0;
		std::string str;
	
		pos = strLine.find_first_of("=");
		if(pos == std::string::npos)
		{
			return false;
		}

		str = strLine.substr(lastpos, pos - lastpos);
	
		return (varMap.find(str) != varMap.end());
	}

}


#endif
