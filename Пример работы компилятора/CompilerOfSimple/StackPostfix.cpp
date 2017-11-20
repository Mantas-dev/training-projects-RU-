#include <cmath>
using std::pow;

#include <iostream>
using namespace std;

#include "StackPostfix.h"

StackPostfix::StackPostfix()
{
	Stack::Stack();
}

StackPostfix::~StackPostfix()
{
	Stack::~Stack();
}

void StackPostfix::push(const int &info)
{
	StackNode *newPtr = Stack::getNewNode(info);

	if (isEmpty())
	{
		firstPtr = lastPtr = newPtr;
	}
	else
	{
		newPtr->nextPtr = firstPtr;
		firstPtr = newPtr;
	}
}

int StackPostfix::pop()
{
	if (!isEmpty())
	{
		StackNode *currentPtr = firstPtr;
		int info = currentPtr->digit;

		firstPtr = currentPtr->nextPtr;

		delete currentPtr;

		return info;
	}
	else
	{
		return 0;
	}
}

int StackPostfix::findSymbol(int &searchSymbol, char type, tableEntry *symbolTable,int &size)
{
	for (int currentRow = 0; currentRow < size + 1; currentRow++)
	{
		if (symbolTable[currentRow].getSymbol() == (char)tolower(searchSymbol) && symbolTable[currentRow].getType() == type)
			return currentRow;
	}

	return 0;
}

void StackPostfix::evaluatePostfixExpression(string *postfix, int sizePostf, tableEntry *tableSymbol,int table_size, int *instrSML, int &instrNum, int varAddress)
{
	StackPostfix StackOfOperrands;

	postfix[sizePostf - 1] = '\0'; // Добавляем нулевой символ в конец массива 

	int i = 0, memCell1 = 0, memCell2 = 0, answer = 0, value = 0;
	int rowTable = 0, postfixSymbol = 0;

	while (postfix[i] != "\0")
	{
		if (atoi(postfix[i].c_str()) == 0 && postfix[i] != "0") //если не число
		{
			postfixSymbol = (int)*postfix[i].c_str();
		}
		else
		{
			postfixSymbol = atoi(postfix[i].c_str());
		}

		if ((rowTable = findSymbol(postfixSymbol, 'V', tableSymbol, table_size)) != 0)
		{
			StackOfOperrands.push(tableSymbol[rowTable].getLocation());
		}
		else if ((rowTable = findSymbol(postfixSymbol, 'C', tableSymbol, table_size)) != 0)
		{
			StackOfOperrands.push(tableSymbol[rowTable].getLocation());
		}
		else
		{
			memCell2 = StackOfOperrands.pop();
			memCell1 = StackOfOperrands.pop();
			value = calculate(memCell1, memCell2, postfix[i], instrSML, instrNum, varAddress);
			instrSML[instrNum] = (21 * 100) + value;
			instrNum++;
			StackOfOperrands.push(value);
		}

		i++;
	}

	instrSML[instrNum - 1] = 0;
	instrNum--;

	++varAddress;

	StackOfOperrands.pop();
}

int StackPostfix::calculate(int &operrand1, int &operrand2, string operat, int *instrSML, int &instrNum, int &varAddress)
{
	int result = 0;
	if (operat == "*")
	{
		instrSML[instrNum] = (20 * 100) + operrand1;
		instrNum++;
		instrSML[instrNum] = (33 * 100) + operrand2;
		instrNum++;
	}
	else if (operat == "/")
	{
		instrSML[instrNum] = (20 * 100) + operrand1;
		instrNum++;
		instrSML[instrNum] = (32 * 100) + operrand2;
		instrNum++;
	}
	else if (operat == "+")
	{
		instrSML[instrNum] = (20 * 100) + operrand1;
		instrNum++;
		instrSML[instrNum] = (30 * 100) + operrand2;
		instrNum++;
	}
	else if (operat == "-")
	{
		instrSML[instrNum] = (20 * 100) + operrand1;
		instrNum++;
		instrSML[instrNum] = (31 * 100) + operrand2;
		instrNum++;
	}
	else if (operat == "%")
	{
		instrSML[instrNum] = (20 * 100) + operrand1;
		instrNum++;
		instrSML[instrNum] = (34 * 100) + operrand2;
		instrNum++;
	}
	else if (operat == "^")
	{
		instrSML[instrNum] = (20 * 100) + operrand1;
		instrNum++;
		instrSML[instrNum] = (35 * 100) + operrand2;
		instrNum++;
	}

	return --varAddress;
}

