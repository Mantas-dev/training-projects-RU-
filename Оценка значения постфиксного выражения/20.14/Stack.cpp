#include <iostream>
using std::cout;

#include "Stack.h";

Stack::Stack()
:firstPtr(0), lastPtr(0)
{

}

Stack::~Stack()
{
	if (!isEmpty())
	{
		StackNode *currentPtr = firstPtr;
		StackNode *tmpPtr;
		
		while (currentPtr != 0)
		{
			tmpPtr = currentPtr;
			currentPtr = currentPtr->nextPtr;
			delete tmpPtr;
		}
	}
}

void Stack::push(const int &info)
{
	StackNode *newPtr = getNewNode(info);

	if (isEmpty())
	{
		firstPtr = lastPtr = newPtr;
	}
	else
	{
		newPtr->nextPtr = firstPtr;
		firstPtr = newPtr;
	}
}

int Stack::pop()
{
	if (!isEmpty())
	{
		StackNode *currentPtr = firstPtr;
		int info = currentPtr->symbol;

		firstPtr = currentPtr->nextPtr;

		delete currentPtr;

		return info;
	}
}

bool Stack::isEmpty() const
{
	return firstPtr == 0;
}

void Stack::printStack() const
{
	if (isEmpty())
	{
		cout << "Stack is empty\n\n";
	}
	else
	{
		StackNode *currentPtr = firstPtr;

		while (currentPtr != 0)
		{
			cout << currentPtr->symbol << ' ';
			currentPtr = currentPtr->nextPtr;
		}

		cout << "\n\n";
	}
}

StackNode * Stack::getNewNode(const int &info)
{
	return new StackNode(info);
}