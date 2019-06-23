#pragma once
#ifndef Type_H
#define Type_H

#include <iostream>
#include <string>
#include <algorithm>
#include <utility>

#include "ErrorLog.h"

struct DataType{
	long long min, max;
	std::string name;
};

struct Var{
	long long min, max;
	long long limMin, limMax;
	std::string name;
};

struct Array {
	size_t nrElem;
	std::string name;
};

bool initialized(const Var& x)
{
	if(x.min == x.limMin and x.max == x.limMax)
	{
		return false;
	}
	return true;
}

int attribute(const Var& base, Var& dest)
{
	if(base.min < dest.limMin or base.max > dest.limMax)
	{
		dest.min = dest.limMin;
		dest.max = dest.limMax;
		return sERROR;
	}
	dest.min = base.min;
	dest.max = base.max;
	return sSUCCESS;

}

/*void verifySum(const Var& a, const Var& b)
{
	if(!initialized(a))
	{
		std::cout << "Variable " << a.name << " uninitialized!\n";
	}
	
	if(!initialized(b))
	{
		std::cout << "Variable " << b.name << " uninitialized!\n";
	}
	
	if(a.min < 0 and b.min < 0)
	{
		long long aux = 
	}
	
}*/

bool operator == (const DataType& a, const DataType& b)
{
	return (a.min == b.min and a.max == b.max and a.name == b.name);
}

bool operator != (const DataType& a, const DataType& b)
{
	return !(a.min == b.min and a.max == b.max and a.name == b.name);
}

Var operator + (const Var& a, const Var& b)
{

	Var aux;
	aux.min = a.min + b.min;
	aux.max = a.max + b.max;
	return aux;
}



Var operator - (const Var& a, const Var& b)
{

	Var aux;
	aux.min = a.min - b.min;
	aux.max = a.max - b.max;
	return aux;
}


Var operator * (const Var& a, const Var& b)
{

	Var aux;
	long long min1, min2, max1, max2;

	min1 = std::min(a.min * b.min, a.min * b.max);
	min2 = std::min(a.max * b.min, a.max * b.max);

	aux.min = std::min(min1, min2);

	max1 = std::max(a.min * b.min, a.min * b.max);
	max2 = std::max(a.max * b.min, a.max * b.max);

	aux.max = std::max(max1, max2);

	return aux;
}


Var operator / (const Var& a, const Var& b)
{

	Var aux;
		long long min1, min2, max1, max2;

		min1 = std::min(a.min / b.min, a.min / b.max);
		min2 = std::min(a.max / b.min, a.max / b.max);

		aux.min = std::min(min1, min2);

		max1 = std::max(a.min / b.min, a.min / b.max);
		max2 = std::max(a.max / b.min, a.max / b.max);

		aux.max = std::max(max1, max2);

		return aux;
}


std::pair<Var, Var> operator == (const Var& a, const long long& b)
{
	Var posInterval, negInterval;

	if (a.min == b && a.max == b)
	{
		posInterval.limMin = a.limMin;
		posInterval.limMax = a.limMax;
		posInterval.min = a.min;
		posInterval.max = a.max;
		negInterval.limMin = 0;
		negInterval.limMax = 0;
		negInterval.min = 0;
		negInterval.max = 0;
	}
	else if (a.min == b)
	{
		posInterval.limMin = a.limMin;
		posInterval.limMax = a.limMax;
		posInterval.min = b;
		posInterval.max = b;
		negInterval.limMin = a.limMin;
		negInterval.limMax = a.limMax;
		negInterval.min = a.min + 1;
		negInterval.max = a.max;
	}
	else if (a.max == b)
	{
		posInterval.limMin = a.limMin;
		posInterval.limMax = a.limMax;
		posInterval.min = b;
		posInterval.max = b;
		negInterval.limMin = a.limMin;
		negInterval.limMax = a.limMax;
		negInterval.min = a.min;
		negInterval.max = a.max - 1;
	}
	else if (b > a.min && b < a.max)
	{
		posInterval.limMin = a.limMin;
		posInterval.limMax = a.limMax;
		posInterval.min = b;
		posInterval.max = b;
		negInterval.limMin = a.limMin;
		negInterval.limMax = a.limMax;
		negInterval.min = a.min;
		negInterval.max = a.max;
	}
	else
	{
		posInterval.limMin = 0;
		posInterval.limMax = 0;
		posInterval.min = 0;
		posInterval.max = 0;
		negInterval.limMin = a.limMin;
		negInterval.limMax = a.limMax;
		negInterval.min = a.min;
		negInterval.max = a.max;
	}
	return std::pair<Var, Var>(posInterval, negInterval);
}

std::pair<Var, Var> operator != (const Var& a, const long long& b)
{
	std::pair<Var, Var> eqResult = (a == b);
	return std::pair<Var, Var>(eqResult.second, eqResult.first);
}

std::pair<Var, Var> operator < (const Var& a, const long long& b)
{
	Var posInterval, negInterval;
	if (a.max < b)
	{
		posInterval.limMax = a.limMax;
		posInterval.limMin = a.limMin;
		posInterval.max = a.max;
		posInterval.min = a.min;
		negInterval.limMax = 0;
		negInterval.limMin = 0;
		negInterval.min = 0;
		negInterval.max = 0;
	}
	else if (a.min >= b)
	{
		negInterval.limMax = a.limMax;
		negInterval.limMin = a.limMin;
		negInterval.max = a.max;
		negInterval.min = a.min;
		posInterval.limMax = 0;
		posInterval.limMin = 0;
		posInterval.min = 0;
		posInterval.max = 0;
	}
	else
	{
		posInterval.limMax = a.limMax;
		posInterval.limMin = a.limMin;
		posInterval.max = b - 1;
		posInterval.min = a.min;
		negInterval.limMax = a.limMax;
		negInterval.limMin = a.limMin;
		negInterval.min = b;
		negInterval.max = a.max;
	}
	return std::pair<Var, Var>(posInterval, negInterval);
}

std::pair<Var, Var> operator >= (const Var& a, const long long& b)
{
	std::pair<Var, Var> eqResult = (a < b);
	return std::pair<Var, Var>(eqResult.second, eqResult.first);
}

std::pair<Var, Var> operator <= (const Var& a, const long long& b)
{
	Var posInterval, negInterval;
	if (a.max <= b)
	{
		posInterval.limMax = a.limMax;
		posInterval.limMin = a.limMin;
		posInterval.max = a.max;
		posInterval.min = a.min;
		negInterval.limMax = 0;
		negInterval.limMin = 0;
		negInterval.min = 0;
		negInterval.max = 0;
	}
	else if (a.min > b)
	{
		negInterval.limMax = a.limMax;
		negInterval.limMin = a.limMin;
		negInterval.max = a.max;
		negInterval.min = a.min;
		posInterval.limMax = 0;
		posInterval.limMin = 0;
		posInterval.min = 0;
		posInterval.max = 0;
	}
	else
	{
		posInterval.limMax = a.limMax;
		posInterval.limMin = a.limMin;
		posInterval.max = b;
		posInterval.min = a.min;
		negInterval.limMax = a.limMax;
		negInterval.limMin = a.limMin;
		negInterval.min = b + 1;
		negInterval.max = a.max;
	}
	return std::pair<Var, Var>(posInterval, negInterval);
}

std::pair<Var, Var> operator > (const Var& a, const long long& b)
{
	std::pair<Var, Var> eqResult = (a <= b);
	return std::pair<Var, Var>(eqResult.second, eqResult.first);
}

template< class myClass>
std::vector<myClass> vecReversal(std::vector<myClass> vec)
{
	myClass auxVar;
	std::vector<myClass> newVec;
	for(auto it = vec.rbegin(); it != vec.rend(); ++it)
	{
		auxVar = *it;
		newVec.push_back(auxVar);
	}

	return newVec;
}


#endif
