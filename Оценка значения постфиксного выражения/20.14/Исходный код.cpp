#include <iostream>
using std::cin;
using std::cout;
using std::endl;

#include <string>
using std::string;
using std::getline;

#include <cmath>
using std::pow;

#include "Stack.h"

int evaluatePostfixExpression(char *arr, int size);
int calculate(int &op1, int &op2, const char &oper);

int main()
{
	
	int length = 0;
	string expr;

	cout << "Enter the expression: ";
	getline(cin, expr);
	cout << endl;
	
	length = expr.length();

	char *postfix = new char[length + 1];

	for (int i = 0; i < length; i++)
		postfix[i] = expr[i];

	postfix[length] = '\0';

	cout << "Expression before converting\n" << expr << endl << endl;


	cout << "Result of expression\n";

	cout << evaluatePostfixExpression(postfix, length + 1) << endl;

	delete [] postfix;

	return 0;
}

int evaluatePostfixExpression(char *postfix, int sizePostf)
{
	Stack StackOfOperrands;

	postfix[sizePostf - 1] = '\0'; // Добавляем нулевой символ в конец массива 

	int i = 0, x = 0, y = 0, answer = 0, value = 0;

	int prevDig = 0, number = 0;

	while (postfix[i] != '\0')
	{
		if (postfix[i] >= 48 & postfix[i] <= 57 & prevDig == 0)
		{
			prevDig = (int)postfix[i] - (int)'0';
		}
		else if (postfix[i] >= 48 & postfix[i] <= 57 & prevDig != 0)
		{
			number = prevDig * 10 + ((int)postfix[i] - (int)'0');
			StackOfOperrands.push(number);
			prevDig = 0;
		}
		else if (postfix[i] == ' ')
		{
			if (prevDig != 0) 
			{
				StackOfOperrands.push(prevDig); 
				prevDig = 0;
			}
		}
		else
		{
			if (prevDig != 0)
			{
				StackOfOperrands.push(prevDig);
				prevDig = 0;
			}
			x = StackOfOperrands.pop();
			y = StackOfOperrands.pop();
			value = calculate(x, y, postfix[i]);

			StackOfOperrands.push(value);
		}

		i++;
	}

	answer = StackOfOperrands.pop();

	return answer;
}

int calculate(int &operrand1, int &operrand2, const char &operat)
{
	int result = 0;
	if (operat == '^') result = pow(operrand2, operrand1);
	else if (operat == '*') result = operrand2 * operrand1;
	else if (operat == '/') result = operrand2 / operrand1;
	else if (operat == '%') result = operrand2 % operrand1;
	else if (operat == '+') result = operrand2 + operrand1;
	else if (operat == '-') result = operrand2 - operrand1;

	return result;
}