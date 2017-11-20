#include "tableEntry.h"

tableEntry::tableEntry()
:symbol(0), location(0), type('\0')
{

}

tableEntry::~tableEntry()
{

}

void tableEntry::setSymbol(int &sym)
{
	symbol = sym;
}

int tableEntry::getSymbol() const
{
	return symbol;
}

void tableEntry::setLocation(int &loc)
{
	location = loc;
}

int tableEntry::getLocation() const
{
	return location;
}

void tableEntry::setType(char t)
{
	type = t;
}

char tableEntry::getType() const
{
	return type;
}