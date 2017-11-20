#include <iostream>
using std::cin;
using std::cout;
using std::endl;

#include <cstdlib>
using std::atoi;

#include <cmath>
using std::pow;

#include <iomanip>
using std::setfill;
using std::showpos;
using std::noshowpos;
using std::setw;
using std::left;
using std::right;
using std::internal;

#include <fstream>
using std::ifstream;
using std::ofstream;
using std::ios_base;

//�������� �����/������
const int READ = 10; //������ ����� � ��������� � ��������� ����� ������
const int WRITE = 11; //������� �� �������� ����� �� ���������� ������ ������

//�������� ��������/��������
const int LOAD = 20; //�������� � ����������� ����� �� ���������� ������ ������
const int STORE = 21; //��������� ����� �� ������������ �� ���������� ������ ������

//�������������� ��������
const int ADD = 30; //��������� �������� ����� � ������������ � ����� �� ���������� �����
					//� ������ (��������� �������� �������� � ������������)
const int SUBTRACT = 31; //�������� �� ����� � ������������ ����� �� ���������� ����� � ������
						 //(��������� �������� �������� � ������������)
const int DIVIDE = 32; //��������� ������� ����� � ������������ �� ����� �� ���������� �����
					   //� ������ (��������� �������� �������� � ������������)
const int MULTIPLY = 33; //��������� ������������ ����� � ������������ � ����� �� ����������
						 //����� � ������ (��������� �������� �������� � ������������)
const int MODULE = 34; //��������� ������ �� ������ ����� � ������������ � ����� �� ����������
						 //����� � ������ (��������� �������� �������� � ������������)
const int POW = 35; //��������� ���������� ����� � ������������ � ������� ����� �� ����������
					   //����� � ������ (��������� �������� �������� � ������������)

//�������� �������� ����������
const int BRANCH = 40; //������� � ���������� ������ ������
const int BRANCHNEG = 41; //������� � ���������� ������ ������, ���� � ������������ ��������� ������������� ����
const int BRANCHZERO = 42; // ������� � ���������� ������ ������, ���� � ������������ ��������� ����
const int HALT = 43; //�������, ����������� ��� ���������� ���������� ����� ������

//�������� ������ ������
const int STROUT = 90; //������� ������ �� �����

//���������
const int SIZE = 100; //������ ������� SML-����������

//�������
int memory[SIZE]; //������, �������� SML-����������

//����������
int accumulator = 0; //����������-�����������. ������ �������� ��������
int instructionCounter = 0; //������ ���������� SML-����������
int operationCode = 0, operand = 0; //operationCode - ������ ��� ����� � SML-����������; operand - ��������� ��� ����� � SML-����������
int instructionRegister = 0; //������ ������� SML-����������

//�����
ifstream inputFile("../Debug/temp_code.sml", ios_base::in); //���� ��� ���������� SML-����������
ofstream outputFile("../Debug/result.txt", ios_base::out); //���� ��� ���������� ���������� ���������� ���������

//�������
void exportIntoFile(ofstream &); //���������� � ���� ��������� ���������� � ���������� ����������

int main()
{
	setlocale(LC_ALL, "Russian");

	char buffer[SIZE]; //������ ������ SML-����������

	bool cycleOn = true; //����������, ���������� �� ���������� ����� �� ���������� SML-���������� 

	int inputWord = 0; //������ �������� ������������� SML-����������

	cout << "*** ��������� ������������ ���! ***\n" << endl;
	
	while (!inputFile.eof()) //���� �� ��������� ����� �����
	{
		inputFile.getline(buffer, SIZE); //��������� SML-���������� �� �����

		inputWord = atoi(buffer); //��������� ���������� � ���� �����

		memory[instructionCounter] = inputWord; //�������� SML-���������� � ������
		++instructionCounter; //��������� ������� SML-���������� � �������
	}
	
	instructionCounter = 0; //������ ��������� �������� ��� ���������� ���������� �� �������

	cout << "*** �������� ��������� ��������� ***\n"
		<< "*** ������� ���������� ��������� ***" << endl;

	while (cycleOn) //���� ��������� ���������� �����
	{
		instructionRegister = memory[instructionCounter]; //��������� SML-���������� � ����������

		operationCode = instructionRegister / 100; //��������� ������ ��� ����� � ����������
		operand = instructionRegister % 100; //��������� ��������� ��� ����� � ����������

		switch (operationCode) //���������� �������� �� ������ ���� ������ SML-����������
		{
		case READ: //�������� ���������� �������� � ��������� � ������
			cout << "? ";
			cin >> memory[operand]; //�������� �������� � ������
			++instructionCounter; //������� � ��������� SML-����������
			break;
		case WRITE: //�������� ������ �������� �� ������ �� �����
			cout << showpos << setw(5) << setfill('0') << internal << memory[operand] << endl; //������� �������� �� �����
			outputFile << showpos << setw(5) << setfill('0') << internal << memory[operand] << endl; //�������� �������� � ����
			cout << setfill(' '); //�������� �������� setfill('0')
			++instructionCounter; //������� � ��������� SML-����������
			break;
		case LOAD: //�������� �������� � ����������� �������� �� �������
			accumulator = memory[operand]; //��������� �������� � ������������
			++instructionCounter; //������� � ��������� SML-����������
			break;
		case STORE: //�������� �������� �� ������������ �������� � ������ 
			memory[operand] = accumulator; //��������� �������� � �������
			accumulator = 0; //�������� �����������
			++instructionCounter; //������� � ��������� SML-����������
			break;
		case ADD: //�������� �������� �������� � ������������ � �������� � �������
			accumulator += memory[operand]; //��������� ��������� �������� � ������������
			++instructionCounter; //������� � ��������� SML-����������
			break;
		case SUBTRACT: //�������� ��������� �� �������� � ������������ �������� � �������
			accumulator -= memory[operand]; //��������� ��������� ��������� � ������������
			++instructionCounter; //������� � ��������� SML-����������
			break;
		case DIVIDE: //�������� ������� �������� � ������������ �� �������� � �������
			if (memory[operand] == 0) //���� �������� � ������� - 0
			{
				//����� ������ �� ����� � � ����
				cout << "*** ������� ������� �� ���� ***\n"
					<< "*** ��������� �������� �������� ���������� ��������� ***" << endl;

				outputFile << "*** ������� ������� �� ���� ***\n"
					<< "*** ��������� �������� �������� ���������� ��������� ***" << endl;

				cycleOn = false; //��������� ���������� ���������� �����
				break;
			}
			//���� �������� � ������� �� ����� ����
			accumulator /= memory[operand]; //��������� ��������� ������� � ������������
			++instructionCounter; //������� � ��������� SML-����������
			break;
		case MULTIPLY: //�������� ��������� �������� � ������������ �� �������� � �������
			accumulator *= memory[operand]; //��������� ��������� ��������� � ������������
			++instructionCounter; //������� � ��������� SML-����������
			break;
		case MODULE: //�������� ������ �� ������ �������� � ������������ � �������� � ������� (�������� "%")
			accumulator %= memory[operand]; //�������� ��������� ������ �� ������ � ������������
			++instructionCounter; //������� � ��������� SML-����������
			break;
		case POW: //�������� ���������� � �������. �������� � ������������ - ���������; �������� � ������� - ���������� �������
			accumulator = pow(accumulator, memory[operand]); //��������� ��������� ���������� � ������� � ������������
			++instructionCounter; //������� � ��������� SML-����������
			break;
		case BRANCH: //�������� �������� � SML-���������� �� ������ 
			instructionCounter = operand; //������� � ��������� SML-����������
			break;
		case BRANCHNEG: //�������� �������� � SML-���������� �� ������
			if (accumulator < 0) //���� �������� � ������������ ������ ����
				instructionCounter = operand; //������� � ��������� SML-����������
			else //���� �������� � ������������ ������ ���� ����� ����
			{
				++instructionCounter; //������� � ��������� SML-����������
			}
			break;
		case BRANCHZERO: //�������� �������� � SML-���������� �� ����
			if (accumulator == 0) //���� �������� � ������������ ����� ����
				instructionCounter = operand; //������� � ��������� SML-����������
			else //���� �������� � ������������ �� ����� ����
			{
				++instructionCounter; //������� � ��������� SML-����������
			}
			break;
		case HALT: //�������� ��������. ��������� ���������� ���������
			cout << "*** ��������� �������� ���� ���������� ***" << endl;

			outputFile << "*** ��������� �������� ���� ���������� ***" << endl;

			cycleOn = false; //��������� ���������� ���������� �����
			break;
		case STROUT: //�������� ������ ������ �� �����
			int strEnd = operand; //��������� ���������� �������� � ������ � ����������
			++instructionCounter; //������� � ��������� SML-����������
			for (int i = 1; i <= strEnd; i++) //������� ������ ������ �� �����
			{
				instructionRegister = memory[instructionCounter]; //��������� SML-���������� � ����������

				operand = instructionRegister % 1000; //���������� ASCII-������������� �������

				cout << char(operand); //������� ������

				++instructionCounter; //������� � ��������� SML-����������
			}
			cout << endl;
			++instructionCounter; //������� � ��������� SML-����������
			break;
		}

	}
	
	//������� �� ����� ��������� ������ ��������� � ���� ������

	cout << "��������:\n"
		<< "accumulator    \t\t " << showpos << setw(5) << setfill('0') << internal << accumulator << "\n"
		<< "instructionCounter    \t    " << setw(2) << setfill('0') << noshowpos << internal << instructionCounter << "\n"
		<< "instructionRegister    \t " << setw(5) << setfill('0') << showpos << internal << instructionRegister << "\n"
		<< "operationCode    \t    " << setw(2) << setfill('0') << noshowpos << operationCode << "\n"
		<< "operand        \t\t    " << setw(2) << setfill('0') << noshowpos << internal << operand << "\n\n"
		<< "������:" << endl;

	//������� �������
	cout << setfill(' ') << setw(8) << right << 0 << " "
		<< setw(5) << 1 << " "
		<< setw(5) << 2 << " "
		<< setw(5) << 3 << " "
		<< setw(5) << 4 << " "
		<< setw(5) << 5 << " "
		<< setw(5) << 6 << " "
		<< setw(5) << 7 << " "
		<< setw(5) << 8 << " "
		<< setw(5) << 9 << " " << endl;
	
	for (int row = 0; row < 91; row += 10) //����� �������� ������� �������� ������� � ���� �������
	{
		cout << noshowpos << setw(2) << row << " "; //������� ����� ������
		for (int i = row; i < row + 10; i++)
		{
			cout << internal << showpos << setw(5) << setfill('0') << memory[i] << " "; //������� ��������
		}
		cout << endl;
	}

	exportIntoFile(outputFile); //�������� ��������� ������ ��������� � ����

	system("pause");

	return 0;
}

void exportIntoFile(ofstream &file) //���������� ��������� ���������� ��������� � ����
{
	file << "��������:\n"
		<< "accumulator    \t\t " << showpos << setw(5) << setfill('0') << internal << accumulator << "\n"
		<< "instructionCounter    \t    " << setw(2) << setfill('0') << noshowpos << internal << instructionCounter << "\n"
		<< "instructionRegister    \t " << setw(5) << setfill('0') << showpos << internal << instructionRegister << "\n"
		<< "operationCode    \t    " << setw(2) << setfill('0') << noshowpos << operationCode << "\n"
		<< "operand        \t\t    " << setw(2) << setfill('0') << noshowpos << internal << operand << "\n\n"
		<< "������:" << endl;

	file << setfill(' ') << setw(8) << right << 0 << " "
		<< setw(5) << 1 << " "
		<< setw(5) << 2 << " "
		<< setw(5) << 3 << " "
		<< setw(5) << 4 << " "
		<< setw(5) << 5 << " "
		<< setw(5) << 6 << " "
		<< setw(5) << 7 << " "
		<< setw(5) << 8 << " "
		<< setw(5) << 9 << " " << endl;

	for (int row = 0; row < 91; row += 10)
	{
		file << noshowpos << setw(2) << row << " ";
		for (int i = row; i < row + 10; i++)
		{
			file << internal << showpos << setw(5) << setfill('0') << memory[i] << " ";
		}
		file << endl;
	}
}