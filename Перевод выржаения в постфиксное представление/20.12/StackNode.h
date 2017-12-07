#ifndef STACKNODE_H
#define STACKNODE_H

class Stack;

class StackNode
{
	friend class Stack;
public:
	StackNode(const char &);
private:
	char symbol;
	StackNode *nextPtr;
};

#endif