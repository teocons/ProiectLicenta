// CPP program to evaluate a given 
// expression where tokens are  
// separated by space. 

#pragma once
#ifndef exprEval_H
#define exprEval_H

#include <iostream>
#include <string>
#include <stack>
#include <cctype>
#include <unordered_map>
#include <utility>

#include "Type.h"
#include "StringHelper.h"
#include "ErrorLog.h"

#define ISPOSONLY 1
#define ISNEGONLY -1
#define ISBOTH 0


namespace Expression
{

	int precedence(char op)
	{
		if(op == '+'||op == '-')
		return 1;
		if(op == '*'||op == '/')
		return 2;
		return 0;
	}

	// Function to perform arithmetic operations.
	Var applyOperation(const Var& a, const Var& b, char op)
	{
		switch(op)
		{
			case '+': return a + b;
			case '-': return a - b;
			case '*': return a * b;
			case '/': return a / b;
		}
	}

	std::pair<Var, Var> makeCompOp(const Var& a, const long long b, const std::string& str)
	{
		if (str == EQUALSIGN)
		{
			return a == b;
		}
		else if (str == DIFFERENTSIGN)
		{
			return a != b;
		}
		else if (str == LESSSIGN)
		{
			return a < b;
		}
		else if (str == LESSEQUALSIGN)
		{
			return a <= b;
		}
		else if (str == GREATERSIGN)
		{
			return a > b;
		}
		else if (str == GREATEREQUALSIGN)
		{
			return a >= b;
		}
	}

	int whichCondCase(std::pair<Var, Var> branchVars)
	{
		if (branchVars.first.limMin == 0 && branchVars.first.limMax == 0)
		{
			return ISNEGONLY;
		}
		else if (branchVars.second.limMin == 0 && branchVars.second.limMax == 0)
		{
			return ISPOSONLY;
		}
		else
		{
			return ISBOTH;
		}
	}




	// Function that returns value of
	// expression after evaluation to parameter outputValue.
	int evaluate(const std::string& tokens, std::unordered_map<std::string, Var>& varMap, Var& outputValue)
	{
		Var auxVal;

		size_t pos = 0, lastpos = 0;

		std::stack <Var> values;

		std::stack <char> ops;

		pos = tokens.find_first_not_of(" ", pos);
		ERRORLOGRETURN(pos != std::string::npos, "Evaluation is empty!", sERROR);

		if (tokens[pos] == '[')
		{
			std::string nrStr;
			long long nrMin, nrMax;
			lastpos = pos + 1;
			pos = tokens.find_first_not_of(" ", lastpos);
			ERRORLOGRETURN(pos != std::string::npos, "Can't parse evaluation!", sERROR);
			lastpos = pos;
			pos = tokens.find_first_of(" ,", lastpos);
			ERRORLOGRETURN(pos != std::string::npos, "Can't parse evaluation!", sERROR);
			nrStr = tokens.substr(lastpos, pos - lastpos);
			ERRORLOGRETURN(StringHelper::isNumber(nrStr), "Error parsing termen: " + nrStr + "!\n", sERROR);
			nrMin = stoll(nrStr, NULL, 10);
			lastpos = pos;
			pos = tokens.find_first_not_of(" ", lastpos);
			ERRORLOGRETURN(tokens[pos] == ',', "Can't parse evaluation!", sERROR);
			lastpos = pos + 1;
			pos = tokens.find_first_not_of(" ", lastpos);
			lastpos = pos;
			pos = tokens.find_first_of(" ]", lastpos);
			nrStr = tokens.substr(lastpos, pos - lastpos);
			ERRORLOGRETURN(StringHelper::isNumber(nrStr), "Error parsing termen: " + nrStr + "!\n", sERROR);
			nrMax = stoll(nrStr, NULL, 10);
			lastpos = pos;
			pos = tokens.find_first_not_of(" ", lastpos);
			ERRORLOGRETURN(tokens[pos] == ']', "Line doesn't end correctly!", sERROR);
			outputValue.min = nrMin;
			outputValue.max = nrMax;
		}

		else
		{
			while (pos < tokens.size())
			{
				pos = tokens.find_first_not_of(" ", pos);
				if (pos == std::string::npos)
				{
					break;
				}

				// Current token is an opening
				// brace, push it to 'ops'
				else if (tokens[pos] == '(')
				{
					ops.push(tokens[pos]);
					++pos;
				}

				// Current token is a number, push
				// it to stack for numbers.
				else if (isdigit(tokens[pos]))
				{
					int val = 0;
					std::string token;

					lastpos = pos;

					pos = tokens.find_first_of(" );+-*/", pos);
					if (pos == std::string::npos)
					{
						pos = tokens.size();
					}

					token = tokens.substr(lastpos, pos - lastpos);

					ERRORLOGRETURN(StringHelper::isNumber(token), "Error parsing termen: " + token + "!\n", sERROR)

					val = strtoll(token.c_str(), NULL, 10);

					auxVal.min = val;
					auxVal.max = val;

					values.push(auxVal);
				}
				// Closing brace encountered, solve
				// entire brace.
				else if (tokens[pos] == ')')
				{
					while (!ops.empty() && ops.top() != '(')
					{
						Var val2 = values.top();
						values.pop();

						Var val1 = values.top();
						values.pop();

						char op = ops.top();
						ops.pop();

						values.push(applyOperation(val1, val2, op));
					}

					// pop opening brace.
					ops.pop();
					++pos;
				}

				// Current token is an operator.
				else if (StringHelper::isOperator(tokens[pos]))
				{
					// While top of 'ops' has same or greater
					// precedence to current token, which
					// is an operator. Apply operator on top
					// of 'ops' to top two elements in values stack.
					while (!ops.empty() && precedence(ops.top())
						>= precedence(tokens[pos])) {
						Var val2 = values.top();
						values.pop();

						Var val1 = values.top();
						values.pop();

						char op = ops.top();
						ops.pop();

						values.push(applyOperation(val1, val2, op));
					}

					// Push current token to 'ops'.
					ops.push(tokens[pos]);
					++pos;
				}

				else
				{
					lastpos = pos;
					pos = tokens.find_first_of(" );+-*/", pos);
					if (pos == std::string::npos)
					{
						pos = tokens.size();
					}

					std::string token = tokens.substr(lastpos, pos - lastpos);

					ERRORLOGRETURN(StringHelper::isGoodVarName(token), "Error parsing word: " + token + "!\n", sERROR);
					ERRORLOGRETURN(varMap.find(token) != varMap.end(), "Variable: " + token + " hadn't been declared!\n", sERROR);

					values.push(varMap[token]);
				}
			}



			// Entire expression has been parsed at this
			// point, apply remaining ops to remaining
			// values.
			while (!ops.empty())
			{
				Var val2 = values.top();
				values.pop();

				Var val1 = values.top();
				values.pop();

				char op = ops.top();
				ops.pop();



				values.push(applyOperation(val1, val2, op));
			}



			// Top of 'values' contains result, return it.
			outputValue = values.top();
		}

		

		return sSUCCESS;
	}
}


#endif
  
