#ifndef STACKNODE_H
#define STACKNODE_H

class Stack;

class StackNode
{
	friend class Stack;
public:
	StackNode(const int &);
private:
	int symbol;
	StackNode *nextPtr;
};

#endif