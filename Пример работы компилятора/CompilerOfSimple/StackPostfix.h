#ifndef STACKPOSTFIX_H
#define STACKPOSTFIX_H

#include <string>
using std::string;

#include "Stack.h"
#include "tableEntry.h"

class StackPostfix : public Stack
{
public:
	StackPostfix();
	~StackPostfix();

	void evaluatePostfixExpression(string *arr, int size, tableEntry *,int tableEntrysize,int *instrSML,int &instrNum, int varAddress);

	int findSymbol(int &, char, tableEntry *, int &);
private:
	void push(const int &);
	int pop();
	int calculate(int &op1, int &op2, string oper, int *instrSML, int &instrNum, int &varAddress);
};

#endif