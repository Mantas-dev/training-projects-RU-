#include "StackNode.h"

StackNode::StackNode(const char &ch)
:symbol(ch), nextPtr(0)
{

}

StackNode::StackNode(const int &dig)
: digit(dig), nextPtr(0)
{

}