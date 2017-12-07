#ifndef STACK_H
#define STACK_H

#include "StackNode.h"

class Stack
{
public:
	Stack();
	~Stack();
	void push(const char &);
	void pop();
	char stackTop() const;
	bool isEmpty() const;
	void printStack() const;
private:
	StackNode *firstPtr;
	StackNode *lastPtr;

	StackNode *getNewNode(const char &);
};

#endif