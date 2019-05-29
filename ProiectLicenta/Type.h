#pragma once
#ifndef Type_H
#define Type_H

#include <iostream>
#include <string>
#include <algorithm>

#include "ErrorLog.h"

struct Type{
	long long min, max;
	std::string name;
	Type(long long x)
	{
		min = x;
		max = x;
		name = "";
	}
	Type() {}
};

struct Var{
	long long min, max;
	long long limMin, limMax;
	std::string name;
};

struct Array {
	size_t nrElem;
	std::string nume;
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

bool operator == (const Type& a, const Type& b)
{
	return (a.min == b.min and a.max == b.max and a.name == b.name);
}

bool operator != (const Type& a, const Type& b)
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
