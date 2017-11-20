#ifndef STACKNODE_H
#define STACKNODE_H

class Stack;
class StackInfix;
class StackPostfix;

class StackNode
{
	friend class Stack;
	friend class StackInfix;
	friend class StackPostfix;
public:
	StackNode(const char &);
	StackNode(const int &);
private:
	char symbol;
	int digit;
	StackNode *nextPtr;
};

#endif