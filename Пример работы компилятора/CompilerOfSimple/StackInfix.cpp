#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
using std::cout;

#include <cstring>
using std::strtok;
using std::strcpy;
using std::strstr;

#include <cstdlib>
using std::atoi;

#include "StackInfix.h"

StackInfix::StackInfix()
{
	Stack::Stack();
}

StackInfix::~StackInfix()
{
	Stack::~Stack();
}

void StackInfix::push(const char &info)
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

void StackInfix::pop()
{
	if (!isEmpty())
	{
		StackNode *currentPtr = firstPtr;

		firstPtr = currentPtr->nextPtr;

		delete currentPtr;
	}
}

char StackInfix::stackTop() const
{
	return firstPtr->symbol;
}

void StackInfix::convertToPostfix(char *infix, int sizeInf, string *postfix, int sizePostf)
{
	StackInfix StackOfOperrands;

	StackOfOperrands.push('('); // Помещаем в стек левую скобку

	infix[sizeInf - 2] = ')'; // Добавляем правую скобку в конец массива 

	char *lexeme = strtok(infix, " ");

	int i = 0; int j = -1;
	int tmpInt = 0;
	char tmpStr[10] = "";

	char* tmpPtr;

	while (!StackOfOperrands.isEmpty()) // Пока стек не пустой
	{
		if (*lexeme == '(')//если левая скобка
		{
			StackOfOperrands.push(*lexeme);

			lexeme[0] = ' ';

			if (lexeme[1] >= 97 && lexeme[1] <= 122)
			{
				j++;
				postfix[j] = lexeme[1];
			}
			else if (lexeme[1] >= 48 && lexeme[1] <= 57)
			{
				j++;
				tmpInt = atoi(lexeme);
				tmpPtr = _itoa(tmpInt, tmpStr, 10);
				postfix[j] = tmpStr;
			}
		}
		else if (*lexeme == ')')
		{
			while (StackOfOperrands.stackTop() != '(')
			{
				j++;
				postfix[j] = StackOfOperrands.stackTop();
				StackOfOperrands.pop();
			}
		}
		else if (atoi(lexeme) != 0 || *lexeme == 48) //если найдено число
		{
			j++;

			tmpInt = atoi(lexeme);
			tmpPtr = _itoa(tmpInt, tmpStr, 10);
			postfix[j] = tmpStr;

			if (strstr(lexeme, ")") != 0) //если найдена правая скобка
			{
				while (StackOfOperrands.stackTop() != '(')
				{
					j++;
					postfix[j] = StackOfOperrands.stackTop();
					StackOfOperrands.pop();
				}
				StackOfOperrands.pop();
			}
		}
		else if (isOperator(lexeme)) //если найден оператор
		{
			if (!StackOfOperrands.isEmpty())
			{
				while (precedence(StackOfOperrands.stackTop(), *lexeme) == 0 ||
					precedence(StackOfOperrands.stackTop(), *lexeme) == 1)
				{
					j++;
					postfix[j] = StackOfOperrands.stackTop();
					StackOfOperrands.pop();
				}
			}

			StackOfOperrands.push(*lexeme);
		}
		else if ((*lexeme >= 97 && *lexeme <= 122) || (*lexeme >= 65 && *lexeme <= 90)) //если найдена переменная
		{
			j++;
			postfix[j] = *lexeme;

			if (strstr(lexeme, ")") != 0) //если найдена правая скобка
			{
				while (StackOfOperrands.stackTop() != '(')
				{
					j++;
					postfix[j] = StackOfOperrands.stackTop();
					StackOfOperrands.pop();
				}
				StackOfOperrands.pop();
			}
		}

		if ((lexeme = strtok(NULL, " ")) == NULL && !StackOfOperrands.isEmpty())
		{
			while (StackOfOperrands.stackTop() != '(')
			{
				j++;
				postfix[j] = StackOfOperrands.stackTop();
				StackOfOperrands.pop();
			}
			StackOfOperrands.pop();
		}
	}

	postfix[j + 1] = "\0";
}

bool StackInfix::isOperator(const char *ch)
{
	return (*ch == '^' || *ch == '*' || *ch == '/' || *ch == '%' || *ch == '+' || *ch == '-');
}

int StackInfix::precedence(const char &operator1, const char &operator2)
{
	if (operator1 == '^')
	{
		if (operator2 != '^') return 1;
		else return 0;
	}
	else if (operator1 == '*' || operator1 == '/' || operator1 == '%')
	{
		if (operator2 == '^') return -1;
		else if (operator2 == '*' || operator2 == '/' || operator2 == '%') return 0;
		else return 1;
	}
	else if (operator1 == '+' || operator1 == '-')
	{
		if (operator2 == '^' || operator2 == '*' || operator2 == '/' || operator2 == '%') return -1;
		else return 0;
	}
}