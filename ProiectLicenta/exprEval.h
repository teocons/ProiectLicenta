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

#include "Type.h"
#include "StringHelper.h"
#include "ErrorLog.h"


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
	Var applyOp(const Var& a, const Var& b, char op)
	{
		switch(op)
		{
			case '+': return a + b;
			case '-': return a - b;
			case '*': return a * b;
			case '/': return a / b;
		}
	}


	// Function that returns value of
	// expression after evaluation to parameter outputValue.
	int evaluate(const std::string& tokens, std::unordered_map<std::string, Var>& varMap, Var& outputValue)
	{
		Var auxVal;

		size_t pos = 0, lastpos = 0;

		// stack to store integer values.
		std::stack <Var> values;

		// stack to store operators.
		std::stack <char> ops;

		while(pos < tokens.size())
		{
			pos = tokens.find_first_not_of(" ", pos);
			if(pos == std::string::npos)
			{
				break;
			}

			// Current token is an opening
			// brace, push it to 'ops'
			else if(tokens[pos] == '(')
			{
				ops.push(tokens[pos]);
				++pos;
			}

			// Current token is a number, push
			// it to stack for numbers.
			else if(isdigit(tokens[pos]))
			{
				int val = 0;
				std::string token;

				lastpos = pos;

				pos = tokens.find_first_of(" );+-*/", pos);
				if(pos == std::string::npos)
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
			else if(tokens[pos] == ')')
			{
				while(!ops.empty() && ops.top() != '(')
				{
					Var val2 = values.top();
					values.pop();

					Var val1 = values.top();
					values.pop();

					char op = ops.top();
					ops.pop();

					values.push(applyOp(val1, val2, op));
				}

				// pop opening brace.
				ops.pop();
				++pos;
			}

			// Current token is an operator.
			else if(StringHelper::isOperator(tokens[pos]))
			{
				// While top of 'ops' has same or greater
				// precedence to current token, which
				// is an operator. Apply operator on top
				// of 'ops' to top two elements in values stack.
				while(!ops.empty() && precedence(ops.top())
									>= precedence(tokens[pos])){
					Var val2 = values.top();
					values.pop();

					Var val1 = values.top();
					values.pop();

					char op = ops.top();
					ops.pop();

					values.push(applyOp(val1, val2, op));
				}

				// Push current token to 'ops'.
				ops.push(tokens[pos]);
				++pos;
			}

			else
			{
				lastpos = pos;
				pos = tokens.find_first_of(" );+-*/", pos);
				if(pos == std::string::npos)
				{
					pos = tokens.size();
				}

				std::string token = tokens.substr(lastpos, pos - lastpos);

				ERRORLOGRETURN(StringHelper::isGoodVarName(token), "Error parsing word: " + token + "!\n", sERROR);
				ERRORLOGRETURN( varMap.find(token) != varMap.end(), "Variable: " + token + " hadn't been declared!\n", sERROR);

				values.push(varMap[token]);
			}
		}



		// Entire expression has been parsed at this
		// point, apply remaining ops to remaining
		// values.
		while(!ops.empty())
		{
			Var val2 = values.top();
			values.pop();

			Var val1 = values.top();
			values.pop();

			char op = ops.top();
			ops.pop();



			values.push(applyOp(val1, val2, op));
		}



		// Top of 'values' contains result, return it.
		outputValue = values.top();

		return sSUCCESS;
	}
}


#endif
  
