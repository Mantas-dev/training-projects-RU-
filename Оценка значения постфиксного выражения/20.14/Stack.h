#ifndef STACK_H
#define STACK_H

#include "StackNode.h"

class Stack
{
public:
	Stack();
	~Stack();
	void push(const int &);
	int pop();
	bool isEmpty() const;
	void printStack() const;
private:
	StackNode *firstPtr;
	StackNode *lastPtr;

	StackNode *getNewNode(const int &);
};

#endif