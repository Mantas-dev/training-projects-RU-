#include <iostream>
using std::cin;
using std::cout;
using std::endl;

#include <string>
using std::string;
using std::getline;

#include "Stack.h"

void convertToPostfix(char *, int, char *, int);
bool isOperator(const char &);
int precedence(const char &, const char &);

int main()
{
	int length = 0;
	int k = 0;
	string expr;

	cout << "Enter the expression: ";
	getline(cin, expr);
	cout << endl;
	
	length = expr.length();

	char *infix = new char[length + 2];
	char *postfix = new char[length + 1];

	for (int i = 0; i < length; i++)
		infix[i] = expr[i];

	infix[length] = '0';
	infix[length + 1] = '\0';

	cout << "Expression before converting\n" << expr << endl << endl;

	convertToPostfix(infix, length + 2, postfix, length+1);

	cout << "Expression after converting\n";

	while (postfix[k] != '\0')
	{
		if (postfix[k - 1] != ' ' & isOperator(postfix[k]) && k != 0)
			cout << ' ';
		if (postfix[k - 1] != ' ' & (postfix[k] >= 48 && postfix[k] <= 57) && k != 0)
			cout << ' ';
		if (postfix[k] == ' ' & postfix[k + 1] == ' ' & k != length)
		{
			k++; cout << postfix[k]; k++;
		}
		else
		{
			cout << postfix[k];
			k++;
		}
	}
	
	cout << endl << endl;

	delete [] infix;
	delete [] postfix;

	return 0;
}

void convertToPostfix(char *infix, int sizeInf, char *postfix, int sizePostf)
{
	Stack StackOfOperrands;

	StackOfOperrands.push('('); // Помещаем в стек левую скобку

	infix[sizeInf-2] = ')'; // Добавляем правую скобку в конец массива 

	int i = 0; int j = -1;

	while (!StackOfOperrands.isEmpty()) // Пока стек не пустой
	{
		if (infix[i] >= 48 && infix[i] <= 57)
		{
			j++;
			postfix[j] = infix[i];
		}
		else if (infix[i] == '(')
			StackOfOperrands.push(infix[i]);
		else if (isOperator(infix[i]))
		{
			if (!StackOfOperrands.isEmpty())
			{
				while (precedence(StackOfOperrands.stackTop(), infix[i]) == 0 ||
					precedence(StackOfOperrands.stackTop(), infix[i]) == 1)
				{
					j++;
					postfix[j] = StackOfOperrands.stackTop();
					StackOfOperrands.pop();
				}
			}

			StackOfOperrands.push(infix[i]);
		}
		else if (infix[i] == ' ')
		{
			j++;
			postfix[j] = infix[i];
		}
 		else // Если текущий элемент - правая скобка
		{
			while (StackOfOperrands.stackTop() != '(')
			{
				j++;
				postfix[j] = StackOfOperrands.stackTop();
				StackOfOperrands.pop();
			}
			StackOfOperrands.pop();
		}

		i++;
	}

	postfix[j + 1] = '\0';
}

bool isOperator(const char &ch)
{
	return (ch == '^' || ch == '*' || ch == '/' || ch == '%' || ch == '+' || ch == '-');
}

int precedence(const char &operator1, const char &operator2)
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