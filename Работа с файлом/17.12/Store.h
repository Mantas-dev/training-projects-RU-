#ifndef STORE_H
#define STORE_H

#include <string>
using std::string;

class Store
{
public:
	Store(int = 0, string = "", int = 0, double = 0.0);
	
	void setProductID(int);
	int getProductID() const;

	void setNameProduct(string);
	string getNameProduct() const;

	void setCountProduct(int);
	int getCountProduct() const;

	void setCostProduct(double);
	double getCostProduct() const;

private:
	int productID;
	char nameProduct[20];
	int countProduct;
	double costProduct;
};

#endif