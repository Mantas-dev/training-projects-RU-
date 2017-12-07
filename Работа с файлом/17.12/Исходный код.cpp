#include <iostream>
using std::cout;
using std::cin;
using std::endl;
using std::ios;
using std::left;
using std::cerr;
using std::right;
using std::fixed;

#include <fstream>
using std::ifstream;
using std::ofstream;
using std::fstream;

#include <iomanip>
using std::setw;
using std::setprecision;

#include <cstdlib>
using std::exit;

#include "Store.h"

enum Choices {INPUT = 1, LIST, UPDATE, DELETE, END};

int enterChoice();
void createFile();
void inputInFile(fstream &);
void listFile(fstream &);
void updateFile(fstream &);
void deleteFromFile(fstream &);

int main()
{

	fstream fileStore("hardware.dat", ios::in | ios::out);
	
	Store store;

	if (!fileStore)
	{
		cerr << "File couldn't be opened!";
		exit(1);
	}
		
		int choice;
				
		while ((choice = enterChoice()) != END)
		{
			switch(choice)
			{
			case INPUT:
				inputInFile(fileStore);
				break;
			case LIST:
				listFile(fileStore);
				break;
			case UPDATE:
				updateFile(fileStore);
				break;
			case DELETE:
				deleteFromFile(fileStore);
				break;
			default:
				cerr << "Incorrect choice!\n";
				break;
			}

			fileStore.clear();
		}
		
	return 0;
}

int enterChoice()
{		
	system("cls");
	int funcChoice;
	cout << "Select choice:\n"
		<< "1 - Input data in file\n"
		<< "2 - View file\n"
		<< "3 - Update data in file\n"
		<< "4 - Delete data from file\n"
		<< "5 - End work\n";
	cin >> funcChoice;
	return funcChoice;
}

void inputInFile(fstream &inputFile)
{
	int ID;
	
	do
	{
		cout << "Input product's ID in range 1 to 100 - ";
		cin >> ID;
	} while (ID < 1 || ID > 100);
	
	inputFile.seekg((ID - 1) * sizeof(Store));

	Store inputStore;

	inputFile.read(reinterpret_cast<char *>(&inputStore), sizeof(Store));
	cout << left << setw(5) << inputStore.getProductID() << setw(20) << inputStore.getNameProduct() << setw(10) << inputStore.getCountProduct() << setw(10) << fixed << setprecision(2) << inputStore.getCostProduct() << "\n";

	if (inputStore.getProductID() == 0)
	{
		char name[20];
		int count;
		double cost;

		cout << "Input product's name, its count and cost:\n"
			<< "Name - ";
		cin >> setw(20) >> name;

		cout << "Count - ";
		cin >> count;

		cout << "Cost - ";
		cin >> cost;

		inputStore.setProductID(ID); 
		inputStore.setNameProduct(name); 
		inputStore.setCountProduct(count); 
		inputStore.setCostProduct(cost);

		inputFile.seekp((inputStore.getProductID() - 1) * sizeof(Store));

		inputFile.write(reinterpret_cast<const char *>(&inputStore), sizeof(Store));

		cout << "\nRecord added:" << endl;
		cout << left << setw(5) << "ID" << setw(20) << "Name" << setw(10) << "Count" << setw(10) << "Cost" << endl;
		cout << left << setw(5) << inputStore.getProductID() << setw(20) << inputStore.getNameProduct() << setw(10) << inputStore.getCountProduct() << setw(10) << fixed << setprecision(2) << inputStore.getCostProduct() << "\n";
	}
	else
	{
		cerr << "Product already have!\n";
		exit(1);
	}
	system("pause");
}

void listFile(fstream &fileList)
{
	Store listStore;
	fileList.seekg(0);
	fileList.read(reinterpret_cast<char *>(&listStore), sizeof(Store));
	cout << left << setw(5) << "ID" << setw(20) << "Name" << setw(10) << "Count" << setw(10) << "Cost" << endl;

	while (!fileList.eof())
	{ 
		if (listStore.getProductID() != 0)
			cout << left << setw(5) << listStore.getProductID() << setw(20) << listStore.getNameProduct() << setw(10) << listStore.getCountProduct() << setw(10) << fixed << setprecision(2) << listStore.getCostProduct() << "\n";
	
		fileList.read(reinterpret_cast<char *>(&listStore), sizeof(Store));
	}
	system("pause");
}

void updateFile(fstream &upFile)
{
	int ID;

	do
	{
		cout << "Input product's ID in range 1 to 100 - ";
		cin >> ID;
	} while (ID < 1 || ID > 100);

	upFile.seekg((ID - 1) * sizeof(Store));
	Store updateStore;

	upFile.read(reinterpret_cast<char *>(&updateStore), sizeof(Store));

	if (updateStore.getProductID() != 0)
	{
		cout << "Record finded" << endl;
		cout << left << setw(5) << "ID" << setw(20) << "Name" << setw(10) << "Count" << setw(10) << "Cost" << endl;
		cout << left << setw(5) << updateStore.getProductID() << setw(20) << updateStore.getNameProduct() << setw(10) << updateStore.getCountProduct() << setw(10) << fixed << setprecision(2) << updateStore.getCostProduct() << "\n";

		cout << "\nInput product's new count and cost:" << endl;

		int count;
		double cost;

		cout << "Count - ";
		cin >> count;

		cout << "Cost - ";
		cin >> cost;

		updateStore.setCountProduct(count);
		updateStore.setCostProduct(cost);

		upFile.seekp((updateStore.getProductID() - 1) * sizeof(Store));

		upFile.write(reinterpret_cast<const char *>(&updateStore), sizeof(Store));

		cout << "\nRecord updated:" << endl;
		cout << left << setw(5) << "ID" << setw(20) << "Name" << setw(10) << "Count" << setw(10) << "Cost" << endl;
		cout << left << setw(5) << updateStore.getProductID() << setw(20) << updateStore.getNameProduct() << setw(10) << updateStore.getCountProduct() << setw(10) << fixed << setprecision(2) << updateStore.getCostProduct() << "\n";
	}
	else
	{
		cerr << "This product didn't found!";
		exit(1);
	}
	system("pause");
}

void deleteFromFile(fstream &delFile)
{
	int ID;

	do
	{
		cout << "Input product's ID in range 1 to 100 - ";
		cin >> ID;
	} while (ID < 1 || ID > 100);

	delFile.seekg((ID - 1) * sizeof(Store));
	Store deleteStore;

	delFile.read(reinterpret_cast<char *>(&deleteStore), sizeof(Store));

	if (deleteStore.getProductID() != 0)
	{
		cout << "Record finded" << endl;
		cout << left << setw(5) << "ID" << setw(20) << "Name" << setw(10) << "Count" << setw(10) << "Cost" << endl;
		cout << left << setw(5) << deleteStore.getProductID() << setw(20) << deleteStore.getNameProduct() << setw(10) << deleteStore.getCountProduct() << setw(10) << fixed << setprecision(2) << deleteStore.getCostProduct() << "\n";

		cout << "\nDo yow want delete information about this product?(y/n) - ";

		char answer;

		cin >> answer;

		if (answer == 'y')
		{

			delFile.seekp((deleteStore.getProductID() - 1) * sizeof(Store));

			deleteStore.setProductID(0);
			deleteStore.setNameProduct("");
			deleteStore.setCountProduct(0);
			deleteStore.setCostProduct(0.00);
			
			delFile.write(reinterpret_cast<const char *>(&deleteStore), sizeof(Store));

			cout << "\nRecord deleted" << endl;
		}
		else
		{
			cout << "Record don't be deleted" << endl;
		}
	}
	else
	{
		cerr << "This product didn't found!";
		exit(1);
	}
	system("pause");
}