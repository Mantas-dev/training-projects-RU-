#define _CRT_SECURE_NO_WARNINGS
#include <string>
using std::string;

#include "Store.h"

Store::Store(int id, string name, int count, double cost)
{
	setProductID(id);
	setNameProduct(name);
	setCountProduct(count);
	setCostProduct(cost);
}

void Store::setProductID(int id)
{
	productID = id;
}

int Store::getProductID() const
{
	return productID;
}

void Store::setNameProduct(string name)
{
	const char *nameValue = name.data();
	int length = name.size();
	length = (length < 21 ? length : 20);
	strncpy(nameProduct, nameValue, length);
	nameProduct[length] = '\0';
}

string Store::getNameProduct() const
{
	return nameProduct;
}

void Store::setCountProduct(int count)
{
	countProduct = count;
}

int Store::getCountProduct() const
{
	return countProduct;
}

void Store::setCostProduct(double cost)
{
	costProduct = cost;
}

double Store::getCostProduct() const
{
	return costProduct;
}