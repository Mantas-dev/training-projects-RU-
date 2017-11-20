#ifndef STACKINFIX_H
#define STACKINFIX_H

#include <string>
using std::string;

#include "Stack.h"

class StackInfix : public Stack
{
public:
	StackInfix();
	~StackInfix();
	void push(const char &);
	void pop();
	char stackTop() const;

	void convertToPostfix(char *, int, string *, int );
	bool isOperator(const char *);
	int precedence(const char &, const char &);
};

#endif