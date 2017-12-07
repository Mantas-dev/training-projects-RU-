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

void Stack::push(const char &info)
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

void Stack::pop()
{
	if (!isEmpty())
	{
		StackNode *currentPtr = firstPtr;

		firstPtr = currentPtr->nextPtr;

		delete currentPtr;
	}
}

char Stack::stackTop() const
{
	return firstPtr->symbol;
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