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

StackNode * Stack::getNewNode(const char &info)
{
	return new StackNode(info);
}

StackNode * Stack::getNewNode(const int &info)
{
	return new StackNode(info);
}