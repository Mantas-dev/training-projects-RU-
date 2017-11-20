#ifndef ENTRY_H
#define ENTRY_H


class tableEntry
{

public:
	tableEntry();
	~tableEntry();

	void setSymbol(int &);
	int getSymbol() const;

	void setType(char);
	char getType() const;

	void setLocation(int &);
	int getLocation() const;

private:
	int symbol, location;
	char type;
};


#endif