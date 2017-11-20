#ifndef STACK_H
#define STACK_H

#include "StackNode.h"

class Stack
{
public:
	Stack();
	~Stack();
	bool isEmpty() const;
	void printStack() const;
protected:
	StackNode *firstPtr;
	StackNode *lastPtr;

	StackNode *getNewNode(const char &);
	StackNode *getNewNode(const int &);
};

#endif