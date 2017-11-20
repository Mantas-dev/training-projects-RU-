#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
using std::cin;
using std::cout;
using std::endl;
using std::cerr;

#include <cctype>
using std::tolower;

#include <cstring>
using std::strlen;
using std::strcpy;
using std::strcmp;
using std::strcat;

#include <fstream>
using std::ifstream;
using std::ofstream;
using std::ios_base;

#include <string>
using std::string;
using std::getline;

#include "StackInfix.h" 
#include "StackPostfix.h" 
#include "tableEntry.h" 

//���������
const int BUFF_SIZE = 100; //������ ������
const int TABLE_SIZE = 100; //������ ������� ��������

//�������
int instructionsSML[TABLE_SIZE] = {0}; //������, ���������� SML-���������� ��� ���������� �� �����������
int flags[TABLE_SIZE]; //������, ���������� ������ ������������� ������ ��� ������ ������� �����������
char buffer[BUFF_SIZE]; //�����
string funcNames[TABLE_SIZE]; //������, ���������� ���� �������, ����������� � ���������

//�������
StackInfix infixExpr; //������ ������, ������������ ������� ��������� �� ��������� ������ � �����������
StackPostfix postfixExpr; //������ ������, ������������ ������ ������������ ���������. ������� SML-���������� ��� ����������
tableEntry symbolTable[TABLE_SIZE]; //������ �������� ������, �������������� ������� ��������. ��������� ������: �������� ������ (����� ������, ASCII - ������������� ����������, ���������), ��� �������, ����� � ������ ������

//����������
bool firstLexeme = true; //�� ����������� �����, ������ ������� ������ ������ ������ �������� �� �������. ���������� ����������, ���� �� ��������� ������ ������� � ������ � ������� ��������� strtok

int strNum = 0; //������� ����� ������ � ���� ���������
int var = 0; //������ ��� ����������� ��������� ��� ����� ������, ����� ���������� ����� � ������� ��������
int code = 0; //������ SML-���������� ��� ���������� ������ � ������
int rowWithValue = 0; //������ ����� ������ �� ������� ��������, ����������� �������� findSymbol

int currentRowTable = 0; //������� ������ �������
int instructionAddress = 0; //����� ����������
int varAddress = TABLE_SIZE; //���������� ����� ��� ����������, ������� ��������� � ��������������� ������� ������� � SML-������������
int curInstrSML = 0; //������ ����� ������� SML-����������

int firstOperand = 0, secondOperand = 0; //������ ��������, ������������ � �������� ��������� if ... goto

//���������
char* instruction; //������ �������, ������������ ���������� strtok
char anchor[BUFF_SIZE]; //��������� �������� ������ ��� ������������� � ������� notEqual

//�����
ifstream inputFile("../Debug/code.smp", ios_base::in); //����, �������� ��������� �� ����������
ofstream outputFile("../Debug/temp_code.sml", ios_base::out); //����, �������� SML-���������� ����� ����������

//�������
void firstProcess(); //������������ ������ ������ �����������
void secondProcess(); //������������ ������ ������ �����������
void equality(); //������� ����������, ������������ ��������� ���� ���������
void notEqual(); //������� ����������, ������������ ����������� ���� ���������
void greaterOrEqual(); //������� ����������, ������������ ������ �� ���� ������� ��� ������ ��� ��� �����
void lowerOrEqual(); //������� ����������, ������������ ������ �� ���� ������� ��� ������ ��� ��� �����
void greater(); //������� ����������, ������������ ������ �� ���� ������� ��� ������
void lower(); //������� ����������, ������������ ������ �� ���� ������� ��� ������ ��� ��� �����
void calculation(); //������� SML-����������, ��������������� ��������������� ���������
void copyInArray(int &, char *, char *); //�������� ������ �� ������ ������� �������� � ������ ������ ��������
void functionExecute(); //������� ����������, ������������ ���� �������
void cycleFor(); //������� ����������, ������������ ���� ����� for
int checkSyntax(); //��������� ��� ��������� �� ������� �������������� ������

bool isOperator(char); //���������, �������� �� ������ �������������� ����������
bool isLogical(char *); //���������, �������� �� ������ ���������� ����������
bool isVar(char); //���������, �������� �� ������ ������ ����������
bool isZero(char); //���������, �������� �� ������ �����

int findSymbol(int &, char); //���� ������ � ������� ��������

int main()
{
	setlocale(LC_ALL, "Russian");

	for (int i = 0; i < TABLE_SIZE; i++) flags[i] = -1; //������������� ��������� �������� flags ��������� -1

	if (checkSyntax() == 0) //���� ��� �������� ���������� �� �������� ������ - ������ ������� ����������
	{
		firstProcess(); //������� ������� firstProcess

		secondProcess(); //������� ������� secondProcess

		cout << "���������� ��������� ��������� ��� ������."<< endl; //������� ��������� �� �������� ����������

		system("pause");

	}

	return 0;
}

//������ ������ �����������
void firstProcess()
{	
	while (!inputFile.eof()) //���� �� ��������� ����� �����
	{
		inputFile.getline(buffer, BUFF_SIZE); //������� ������ �� �����

		strcpy(anchor, buffer); //���������� ��������� ������

		instruction = strtok(buffer, " "); //������� ������ �������

		firstLexeme = true; //�������, ��� ������� �������� ������ � ������

		while (instruction != NULL) //���� ��� ������ �� ��������� �� �������
		{
			if (firstLexeme) //���� ������� ������ �������
			{
				strNum = atoi(instruction); //��������� ����� ������

				if ((rowWithValue = findSymbol(strNum, 'L')) == -1) //���� ������� ����� ������ �� ������ � ������� ��������
				{
					symbolTable[currentRowTable].setSymbol(strNum); //��������� ����� ������
				}
				firstLexeme = false; //��������� ����������� ������� �� ������ �� �������� ������
			}

			if (strcmp(instruction,"rem") == 0) //���������� ������� - "rem". ��������� ����� ������ ������������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������
				break; //����� �� ����� �� ���������� ������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "input") == 0) //���� ������. ����� input ���� ������ ����������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				instruction = strtok(NULL, " "); //������� ��������� �������

				while (instruction != NULL) //���� �� ������� ��� ������
				{
					var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
					{
						currentRowTable++; //������� �� ��������� ������ � �������
						varAddress--; //���������� ����� ����� � ������ ��� ����������

						//���������� ����� ���������� � ������� ������ � ��������� �����������
						symbolTable[currentRowTable].setSymbol(var); 
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						code = (10 * 100) + varAddress; //��������� SML-���������� ����� ����������

						instructionsSML[curInstrSML] = code; //�������� SML-���������� � ������

						curInstrSML++; //������� �� ������ ������� � ������� SML-����������
					}
					else //���� ������� � �������
					{
						code = (10 * 100) + symbolTable[rowWithValue].getLocation(); //��������� SML-���������� ����� ����������

						instructionsSML[curInstrSML] = code; //�������� SML-���������� � ������

						curInstrSML++; //������� �� ������ ������� � ������� SML-����������
					}

					instruction = strtok(NULL, " "); //������� ��������� �������
				}

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			else if (strcmp(instruction, "if") == 0) //�������� ��������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				instruction = strtok(NULL, " "); //������� ������ �������

				if (atoi(instruction) != 0 || isZero(instruction[0])) //������ ������� �������� ������
				{
					var = atoi(instruction); //��������� ����� � ����������
					
					if ((rowWithValue = findSymbol(var, 'C')) == -1) //���� ��������� �� ������� � ������� ��������
					{
						currentRowTable++; //������� �� ��������� ������ � �������
						varAddress--; //���������� ����� ����� � ������ ��� ���������

						//���������� ��������� � ������� ������ � ��������� �����������
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('C');
						symbolTable[currentRowTable].setLocation(varAddress);

						instructionsSML[varAddress] = var; //��������� �������� ��������� � ������� SML-����������

						firstOperand = varAddress; //��������� � ���������� ����� ������� �������� 
					}
					else firstOperand = symbolTable[rowWithValue].getLocation(); //���� ��������� ������� - ��������� � ���������� ����� ������� �������� 
				}
				else //������ ������� �������� ����������
				{
					var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
					{
						currentRowTable++; //������� �� ��������� ������ � �������
						varAddress--; //���������� ����� ����� � ������ ��� ����������

						//���������� ����� ���������� � ������� ������ � ��������� �����������
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						firstOperand = varAddress; //��������� � ���������� ����� ������� ��������
					}
					else firstOperand = symbolTable[rowWithValue].getLocation(); //���� ���������� ������� - ��������� � ���������� ����� ������� ��������
				}

				instruction = strtok(NULL, " "); //������� ��������� �������

				if (strcmp(instruction, "==") == 0) //���� ������� - "=="
				{
					equality(); //������� ������� equality
				}
				else if (strcmp(instruction, "!=") == 0) //���� ������� - "!="
				{
					notEqual(); //������� ������� notEqual
				}
				else if (strcmp(instruction, ">=") == 0) //���� ������� - ">="
				{
					greaterOrEqual(); //������� ������� greaterOrEqual
				}
				else if (strcmp(instruction, "<=") == 0) //���� ������� - "<="
				{
					lowerOrEqual(); //������� ������� lowerOrEqual
				}
				else if (strcmp(instruction, ">") == 0) //���� ������� - '>'
				{
					greater(); //������� ������� greater
				}
				else if (strcmp(instruction, "<") == 0) //���� ������� - '<'
				{
					lower(); //������� ������� lower
				}
				
				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "let") == 0) //��������� ���������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				calculation(); //������� ������� calculation

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "goto") == 0) //������� �� ������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				instruction = strtok(NULL, " "); //������� ����� ��������. ������� �������� ������ ����� ������.

				var = atoi(instruction); //��������� ����� ������ � ����������

				if ((rowWithValue = findSymbol(var, 'L')) == -1) //���� ����� ������ �� ������ � �������, �� ����� ������ ��������� �������������. ����� ������� ���������� �������� ��� ������. �� ����� ������� ������� ���
																//������ ����� ����������������
				{
					flags[curInstrSML] = var; //��������� � ��������, ��������������� ������� SML-����������, ������� flags ����� ������ ��� �������� 

					code = (40 * 100); //������� ���������� �������� ��� ������

					instructionsSML[curInstrSML] = code; //�������� ����������

					curInstrSML++; //������� �� ������ ������� � ������� SML-����������
				}
				else //���� ����� ������ ������ � �������
				{
					code = (40 * 100) + symbolTable[rowWithValue].getLocation(); //������� ���������� �������� �� ������

					instructionsSML[curInstrSML] = code; //�������� ����������

					curInstrSML++; //������� �� ������ ������� � ������� SML-����������
				}

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "print") == 0) //����� ������ ��� �������� ���������� �� �����
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				instruction = strtok(NULL, " "); //������� �������

				while (instruction != NULL) //���� �� ��������� ��� ������
				{
					if (instruction[0] == '"') //���� ������ ������ ������� - ������� �������
					{
						char printString[100] = ""; //������� ���������� ������ ��� �������� ���� ������

						strcat(printString, instruction); //��������� ������� � �������
						instruction = strtok(NULL, ""); //��������� ��������� ������ �������
						if (instruction != NULL)
						{
							strcat(printString, " "); //������� ������ � ������
							strcat(printString, instruction); //��������� ������� � �������
						}

						int length = strlen(printString); //���������� ����� ������

						instructionsSML[curInstrSML] = (90 * 100) + (length - 2); //������� SML-���������� ��� ������� ����� ������ ��� ������� �������
						curInstrSML++; //������� �� ������ ������� � ������� SML-����������

						for (int i = 1; i < length; i++) //��������� ���������� ������ ��� ������� �������
						{
							instructionsSML[curInstrSML] = (90 * 100) + (int)printString[i];
							curInstrSML++;
						}
					}
					else //��������� ����������
					{
						var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

						if ((rowWithValue = findSymbol(var, 'V')) != -1) //���� ���������� ������� � ������� ��������
						{
							code = (11 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ������ �������� ����������

							instructionsSML[curInstrSML] = code; //�������� ����������

							curInstrSML++; //������� �� ������ ������� � ������� SML-����������
						}
						else if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
						{
							currentRowTable++; //������� �� ��������� ������ � �������
							varAddress--; //���������� ����� ����� � ������ ��� ����������

							//���������� ����� ���������� � ������� ������ � ��������� �����������
							symbolTable[currentRowTable].setSymbol(var);
							symbolTable[currentRowTable].setType('V');
							symbolTable[currentRowTable].setLocation(varAddress);

							code = (11 * 100) + varAddress; //������� SML-���������� ������ �������� ����������

							instructionsSML[curInstrSML] = code; //�������� ����������

							curInstrSML++; //������� �� ������ ������� � ������� SML-����������
						}
					}

					instruction = strtok(NULL, " "); //��������� ��������� �������
				}

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "end") == 0) //���������� ���������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				instructionsSML[curInstrSML] = 4300; //������� ���������� ���������� ���������� ���������
				curInstrSML++; //������� �� ������ ������� � ������� SML-����������

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			else if (strcmp(instruction, "gosub") == 0) //�������� �������� � �������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				functionExecute(); //����� ������� functionExecute

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			else if (strcmp(instruction, "sub") == 0) //���������� �������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				while (strcmp(instruction, "return") != 0) //���� �� �������� �������� "return"
				{
					instruction = strtok(NULL, " "); //��������� ��������� �������

					if (instruction == NULL) //���� ������ ���������
					{
						inputFile.getline(buffer, BUFF_SIZE); //��������� ����� ������ �� �����
						instruction = strtok(buffer, " "); //������� �������
					}
				}
			}

			else if (strcmp(instruction, "for") == 0) //���� for
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				cycleFor(); //����� ������� cycleFor

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			instruction = strtok(NULL, " "); //������� ��������� �������
		}

		if (currentRowTable > TABLE_SIZE) //���� ������� ������ ������� ������� �� ������������� �������
		{
			cerr << "��������� ������������ ������� ��������." << endl; //������ ������
			system("pause");
			system("exit"); //����� �� ���������
		}
		else
		{
			currentRowTable++; //������� � ��������� ������ � ������� �������� 
		}
	}
}

//������ ������ �����������
void secondProcess()
{
	for (int i = 0; i < TABLE_SIZE; i++) //������ �� ���� ��������� ������� flags
	{
		if (flags[i] != -1) //���� ����������� ������� �������� �� -1
		{
			if ((rowWithValue = findSymbol(flags[i], 'L')) != -1) //���� ����� ������, ������������ � flags[i], ������
			{
				instructionsSML[i] += symbolTable[rowWithValue].getLocation(); //�������� � ������ ���������� �������� ����� ��������
			}
		}
	}

	outputFile << instructionsSML[0]; //�������� � ���� ������ ����������

	for (int i = 1; i < TABLE_SIZE; i++) //�������� � ���� ��������� ���������� � �������������� ������ �������� �� ����� ������
	{
		outputFile << "\n" << instructionsSML[i];
	}
}

//����� �������
int findSymbol(int &searchSymbol, char type)
{
	for (int currentRow = 0; currentRow < TABLE_SIZE + 1; currentRow++) //������ �� ���� ������� ��������
	{
		if (symbolTable[currentRow].getSymbol() == (char)tolower(searchSymbol) && symbolTable[currentRow].getType() == type) //���� ������ ��������� ������ � �������� ����� - ������� ������� � �������
			return currentRow;
	}
	return -1; //���� ������ �� ������ - ������� -1
}

//�������� SML-����������, ������������ ��������� ���� ���������
void equality()
{
	code = (20 * 100) + varAddress; //������� SML-���������� �������� � ����������� ������ ��������

	instructionsSML[curInstrSML] = code; //�������� ����������

	curInstrSML++; //������� �� ������ ������� � ������� SML-����������

	instruction = strtok(NULL, " "); //������� ������ �������

	if (atoi(instruction) != 0 || isZero(instruction[0])) //���� ������ ������� - �����
	{
		var = atoi(instruction); //��������� ����� � ����������

		if ((rowWithValue = findSymbol(var, 'C')) == -1) //���� ����� �� �������� � �������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ���������

			//���������� ��������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ��������� ������� �������� �� �������

			instructionsSML[curInstrSML] = code; //�������� ����������
		
			instructionsSML[varAddress] = var; //�������� �������� ���������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
		else //���� ����� ������� � �������
		{
			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ��������� ������� �������� �� �������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
	}
	else //���� ������ ������� - ���������� 
	{
		var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

		if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ����������

			//���������� ���������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('V');
			symbolTable[currentRowTable].setLocation(varAddress);

			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ��������� ������� �������� �� �������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
		else //���� ���������� ������� � �������
		{
			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ��������� ������� �������� �� �������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
	}

	instruction = strtok(NULL, " "); //���������� goto
	instruction = strtok(NULL, " "); //���������� ������ ��������

	var = atoi(instruction); //��������� ����� ������ � ����������

	if ((rowWithValue = findSymbol(var, 'L')) == -1) //���� ����� ������ �� ������ � �������, �� ����� ������ ��������� �������������. ����� ������� ���������� �������� ��� ������. �� ����� ������� ������� ���
													 //������ ����� ����������������
	{
		flags[curInstrSML] = var; //��������� � ��������, ��������������� ������� SML-����������, ������� flags ����� ������ ��� �������� 

		code = (42 * 100); //������� SML-���������� �������� �� ���� ��� ������

		instructionsSML[curInstrSML] = code; //�������� ����������

		curInstrSML++; //������� �� ������ ������� � ������� SML-����������
	}
	else //���� ����� ������ ������ � �������
	{
		code = (42 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� �������� �� ����

		instructionsSML[curInstrSML] = code; //�������� ����������

		curInstrSML++; //������� �� ������ ������� � ������� SML-����������
	}
}

//�������� SML-����������, ������������ ����������� ���� ���������
void notEqual()
{
	greater(); //����� ������� greater

	//������� ��������� ��������� ������ �� ������� �� ������� �������� � ���������� ���������
	instruction = strtok(anchor, " "); 
	instruction = strtok(NULL, " ");
	instruction = strtok(NULL, " ");
	instruction = strtok(NULL, " ");

	lower(); //����� ������� greater
}

//�������� SML-����������, ������������ ������ �� ���� ������� ��� ������ ��� ��� �����
void greaterOrEqual()
{
	instruction = strtok(NULL, " "); //����������� ������� ��������

	if (atoi(instruction) != 0 || isZero(instruction[0])) //���� ������ ������� - �����
	{
		secondOperand = atoi(instruction); //��������� ����� � ����������

		if ((rowWithValue = findSymbol(secondOperand, 'C')) == -1) //���� ����� �� ������� � ������� ��������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ���������

			//���������� ��������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(secondOperand);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = secondOperand; //�������� �������� ���������

			code = (20 * 100) + varAddress; //������� SML-���������� �������� � ����������� ������� ��������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
		else //���� ����� ������� � �������
		{
			code = (20 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� �������� � ����������� ������� ��������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
	}
	else //���� ������ ������� - ����������
	{
		secondOperand = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

		if ((rowWithValue = findSymbol(secondOperand, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ����������

			//���������� ���������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(secondOperand);
			symbolTable[currentRowTable].setType('V');
			symbolTable[currentRowTable].setLocation(varAddress);

			code = (20 * 100) + varAddress; //������� SML-���������� �������� � ����������� ������� ��������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
		else
		{
			code = (20 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� �������� � ����������� ������� ��������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
	}

	code = (31 * 100) + firstOperand; //������� SML-���������� ��������� ������� �������� �� �������

	instructionsSML[curInstrSML] = code; //�������� ����������

	curInstrSML++; //������� �� ������ ������� � ������� SML-����������

	instruction = strtok(NULL, " "); //���������� goto
	instruction = strtok(NULL, " "); //���������� ������ ��������

	var = atoi(instruction); //��������� ����� ������ � ����������

	if ((rowWithValue = findSymbol(var, 'L')) == -1) //���� ����� ������ �� ������ � �������, �� ����� ������ ��������� �������������. ����� ������� ���������� �������� ��� ������. �� ����� ������� ������� ���
													 //������ ����� ���������������� 
	{
		flags[curInstrSML] = var; //��������� � ��������, ��������������� ������� SML-����������, ������� flags ����� ������ ��� ��������

		code = (41 * 100); //������� SML-���������� �������� �� ������ ��� ������

		instructionsSML[curInstrSML] = code; //�������� ����������

		curInstrSML++; //������� �� ������ ������� � ������� SML-����������

		flags[curInstrSML] = var; //��������� � ��������, ��������������� ������� SML-����������, ������� flags ����� ������ ��� ��������

		code = (42 * 100); //������� SML-���������� �������� �� ���� ��� ������

		instructionsSML[curInstrSML] = code; //�������� ����������

		curInstrSML++; //������� �� ������ ������� � ������� SML-����������
	}
	else
	{
		code = (41 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� �������� �� ������

		instructionsSML[curInstrSML] = code; //�������� ����������

		curInstrSML++; //������� �� ������ ������� � ������� SML-����������

		code = (42 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� �������� �� ����

		instructionsSML[curInstrSML] = code; //�������� ����������

		curInstrSML++; //������� �� ������ ������� � ������� SML-����������
	}
}

//�������� SML-����������, ������������ ������ �� ���� ������� ��� ������ ��� ��� �����
void lowerOrEqual()
{
	code = (20 * 100) + firstOperand; //������� SML-���������� �������� � ����������� ������ ��������

	instructionsSML[curInstrSML] = code; //�������� ����������

	curInstrSML++; //������� �� ������ ������� � ������� SML-����������

	instruction = strtok(NULL, " "); //����������� ������� ��������

	if (atoi(instruction) != 0 || isZero(instruction[0])) //���� ������ ������� - �����
	{
		var = atoi(instruction); //��������� ����� � ����������

		if ((rowWithValue = findSymbol(var, 'C')) == -1) //���� ����� �� ������� � �������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ���������

			//���������� ��������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = var; //�������� �������� ���������

			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ��������� ������� �������� �� �������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
		else //����� ������� � �������
		{
			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ��������� ������� �������� �� �������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
	}
	else //���� ������ ������� - ����������
	{
		var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

		if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ����������

			//���������� ���������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('V');
			symbolTable[currentRowTable].setLocation(varAddress);

			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ��������� ������� �������� �� �������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
		else
		{
			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ��������� ������� �������� �� �������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
	}

	instruction = strtok(NULL, " "); //���������� goto
	instruction = strtok(NULL, " "); //���������� ������ ��������

	var = atoi(instruction); //��������� ����� ������ � ����������

	if ((rowWithValue = findSymbol(var, 'L')) == -1) //���� ����� ������ �� ������ � �������, �� ����� ������ ��������� �������������. ����� ������� ���������� �������� ��� ������. �� ����� ������� ������� ���
													 //������ ����� ���������������� 
	{
		flags[curInstrSML] = var; //��������� � ��������, ��������������� ������� SML-����������, ������� flags ����� ������ ��� ��������

		code = (41 * 100); //������� SML-���������� �������� �� ������ ��� ������

		instructionsSML[curInstrSML] = code; //�������� ����������

		curInstrSML++; //������� �� ������ ������� � ������� SML-����������

		flags[curInstrSML] = var; //��������� � ��������, ��������������� ������� SML-����������, ������� flags ����� ������ ��� ��������

		code = (42 * 100); //������� SML-���������� �������� �� ���� ��� ������

		instructionsSML[curInstrSML] = code; //�������� ����������

		curInstrSML++; //������� �� ������ ������� � ������� SML-����������
	}
	else
	{
		code = (41 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� �������� �� ������

		instructionsSML[curInstrSML] = code; //�������� ����������

		curInstrSML++; //������� �� ������ ������� � ������� SML-����������

		code = (42 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� �������� �� ����

		instructionsSML[curInstrSML] = code; //�������� ����������

		curInstrSML++; //������� �� ������ ������� � ������� SML-����������
	}
}

//�������� SML-����������, ������������ ������ �� ���� ������� ��� ������
void greater()
{
	instruction = strtok(NULL, " "); //����������� ������� ��������

	if (atoi(instruction) != 0 || isZero(instruction[0])) //���� ������ ������� - �����
	{
		secondOperand = atoi(instruction); //��������� ����� � ����������

		if ((rowWithValue = findSymbol(secondOperand, 'C')) == -1) //���� ����� �� ������� � ������� ��������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ���������

			//���������� ��������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(secondOperand);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = secondOperand; //�������� �������� ���������

			code = (20 * 100) + varAddress; //������� SML-���������� �������� � ����������� ������� ��������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
		else //����� ������� � �������
		{
			code = (20 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� �������� � ����������� ������� ��������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
	}
	else //���� ������ ������� - ����������
	{
		secondOperand = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

		if ((rowWithValue = findSymbol(secondOperand, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ����������

			//���������� ���������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(secondOperand);
			symbolTable[currentRowTable].setType('V');
			symbolTable[currentRowTable].setLocation(varAddress);

			code = (20 * 100) + varAddress; //������� SML-���������� �������� � ����������� ������� ��������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
		else
		{
			code = (20 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� �������� � ����������� ������� ��������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
	}

	code = (31 * 100) + firstOperand; //������� SML-���������� ��������� ������� �������� �� �������

	instructionsSML[curInstrSML] = code; //�������� ����������

	curInstrSML++; //������� �� ������ ������� � ������� SML-����������

	instruction = strtok(NULL, " "); //���������� goto
	instruction = strtok(NULL, " "); //���������� ������ ��������

	var = atoi(instruction); //��������� ����� ������ � ����������

	if ((rowWithValue = findSymbol(var, 'L')) == -1) //���� ����� ������ �� ������ � �������, �� ����� ������ ��������� �������������. ����� ������� ���������� �������� ��� ������. �� ����� ������� ������� ���
													 //������ ����� ���������������� 
	{
		flags[curInstrSML] = var; //��������� � ��������, ��������������� ������� SML-����������, ������� flags ����� ������ ��� ��������

		code = (41 * 100); //������� SML-���������� �������� �� ������ ��� ������

		instructionsSML[curInstrSML] = code; //�������� ����������

		curInstrSML++; //������� �� ������ ������� � ������� SML-����������
	}
	else
	{
		code = (41 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� �������� �� ������

		instructionsSML[curInstrSML] = code; //�������� ����������

		curInstrSML++; //������� �� ������ ������� � ������� SML-����������
	}
}

//�������� SML-����������, ������������ ������ �� ���� ������� ��� ������
void lower()
{
	code = (20 * 100) + firstOperand; //������� SML-���������� �������� � ����������� ������ ��������

	instructionsSML[curInstrSML] = code; //�������� ����������

	curInstrSML++; //������� �� ������ ������� � ������� SML-����������

	instruction = strtok(NULL, " "); //����������� ������� ��������

	if (atoi(instruction) != 0 || isZero(instruction[0])) //���� ������ ������� - �����
	{
		var = atoi(instruction); //��������� ����� � ����������

		if ((rowWithValue = findSymbol(var, 'C')) == -1) //���� ����� �� ������� � ������� ��������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ���������

			//���������� ��������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = var; //�������� �������� ���������

			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ��������� ������� �������� �� �������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++;
		}
		else //����� ������� � �������
		{
			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ��������� ������� �������� �� �������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
	}
	else //���� ������ ������� - ����������
	{
		var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

		if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ����������

			//���������� ���������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('V');
			symbolTable[currentRowTable].setLocation(varAddress);

			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ��������� ������� �������� �� �������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
		else
		{
			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ��������� ������� �������� �� �������

			instructionsSML[curInstrSML] = code; //�������� ����������

			curInstrSML++; //������� �� ������ ������� � ������� SML-����������
		}
	}

	instruction = strtok(NULL, " "); //���������� goto
	instruction = strtok(NULL, " "); //���������� ������ ��������

	var = atoi(instruction); //��������� ����� ������ � ����������

	if ((rowWithValue = findSymbol(var, 'L')) == -1) //���� ����� ������ �� ������ � �������, �� ����� ������ ��������� �������������. ����� ������� ���������� �������� ��� ������. �� ����� ������� ������� ���
													 //������ ����� ���������������� 
	{
		flags[curInstrSML] = var; //��������� � ��������, ��������������� ������� SML-����������, ������� flags ����� ������ ��� ��������

		code = (41 * 100); //������� SML-���������� �������� �� ������ ��� ������

		instructionsSML[curInstrSML] = code; //�������� ����������

		curInstrSML++; //������� �� ������ ������� � ������� SML-����������
	}
	else
	{
		code = (41 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� �������� �� ������

		instructionsSML[curInstrSML] = code; //�������� ����������

		curInstrSML++; //������� �� ������ ������� � ������� SML-����������
	}
}

//�������� SML-����������, ��������������� ��������������� ���������
void calculation()
{
	int length = 0; //����� ������

	bool readEqualSign = false; //����������, ��� �� �������� ���� '='

	char* tmpArray = new char[50]; //��������� ���������� ������, ��� �������� ��������������� ���������

	char varBeforeEqualSign = ' '; //����������, ����������� �� ����� '='

	int currentItem = 0; // �������� �������, ������� ������ ������� ������� �������

	instruction = strtok(NULL, " "); //������� ����������, ����������� ����� ������ '='

	while (instruction != NULL) //���� �� ��������� ��� ������
	{
		if (instruction[0] == '(')
		{
			int length = strlen(instruction) - 1;

			char* tmpInstr = new char[length]; 

			for (int i = 1; i < strlen(instruction); i++) //����������� ���������� ����� ������ �� ��������� ������
				tmpInstr[i - 1] = instruction[i];

			if (atoi(tmpInstr) != 0 || isZero(tmpInstr[0])) //���� ������� - �����
			{
				var = atoi(tmpInstr); //��������� ����� � ����������

				if ((rowWithValue = findSymbol(var, 'C')) == -1) //���� ��������� �� ������� � ������� ��������
				{
					currentRowTable++; //������� �� ��������� ������ � �������
					varAddress--; //���������� ����� ����� � ������ ��� ���������

					//���������� ��������� � ������� ������ � ��������� �����������
					symbolTable[currentRowTable].setSymbol(var);
					symbolTable[currentRowTable].setType('C');
					symbolTable[currentRowTable].setLocation(varAddress);

					instructionsSML[varAddress] = var; //�������� �������� ���������

					if (readEqualSign) //���� ���� '=' ��� �������� �����
					{
						copyInArray(currentItem, tmpArray, tmpInstr); //����������� ������� ������� �� ��������� ������
					}
				}
				else if (readEqualSign) //���� ��������� ������� � ���� ���� '=' ��� �������� �����
				{
					copyInArray(currentItem, tmpArray, tmpInstr); //����������� ������� ������� �� ��������� ������
				}
			}
			else //���� ������� - ���������� ��� ��������
			{
				if (isOperator(tmpInstr[0])) //���� ������� - ��������
				{
					if (readEqualSign) //���� ���� '=' ��� �������� �����
					{
						copyInArray(currentItem, tmpArray, tmpInstr); //����������� ������� ������� �� ��������� ������
					}
				}
				else if (isVar(tmpInstr[0])) //���� ������� - ����������
				{
					var = tolower(tmpInstr[0]); //��������� ASCII-��� �������� ����������

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
					{
						currentRowTable++; //������� �� ��������� ������ � �������
						varAddress--; //���������� ����� ����� � ������ ��� ����������

						//���������� ���������� � ������� ������ � ��������� �����������
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						if (readEqualSign) //���� ���� '=' ��� �������� �����
						{
							copyInArray(currentItem, tmpArray, tmpInstr); //����������� ������� ������� �� ��������� ������
						}

					}
					else if (readEqualSign) //���� ���������� ������� � ���� ���� '=' ��� �������� �����
					{
						copyInArray(currentItem, tmpArray, tmpInstr); //����������� ������� ������� �� ��������� ������
					}
				}
			}
			delete[] tmpInstr;
		}
		else
		{
			if (atoi(instruction) != 0 || isZero(instruction[0])) //���� ������� - �����
			{
				var = atoi(instruction); //��������� ����� � ����������

				if ((rowWithValue = findSymbol(var, 'C')) == -1) //���� ��������� �� ������� � ������� ��������
				{
					currentRowTable++; //������� �� ��������� ������ � �������
					varAddress--; //���������� ����� ����� � ������ ��� ���������

					//���������� ��������� � ������� ������ � ��������� �����������
					symbolTable[currentRowTable].setSymbol(var);
					symbolTable[currentRowTable].setType('C');
					symbolTable[currentRowTable].setLocation(varAddress);

					instructionsSML[varAddress] = var; //�������� �������� ���������

					if (readEqualSign) //���� ���� '=' ��� �������� �����
					{
						copyInArray(currentItem, tmpArray, instruction); //����������� ������� ������� �� ��������� ������
					}
				}
				else if (readEqualSign) //���� ��������� ������� � ���� ���� '=' ��� �������� �����
				{
					copyInArray(currentItem, tmpArray, instruction); //����������� ������� ������� �� ��������� ������
				}
			}
			else //���� ������� - ���������� ��� ��������
			{
				if (isOperator(instruction[0])) //���� ������� - ��������
				{
					if (readEqualSign) //���� ���� '=' ��� �������� �����
					{
						copyInArray(currentItem, tmpArray, instruction); //����������� ������� ������� �� ��������� ������
					}
				}
				else if (isVar(instruction[0])) //���� ������� - ����������
				{
					var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
					{
						currentRowTable++; //������� �� ��������� ������ � �������
						varAddress--; //���������� ����� ����� � ������ ��� ����������

						//���������� ���������� � ������� ������ � ��������� �����������
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						if (readEqualSign) //���� ���� '=' ��� �������� �����
						{
							copyInArray(currentItem, tmpArray, instruction); //����������� ������� ������� �� ��������� ������
						}

					}
					else if (readEqualSign) //���� ���������� ������� � ���� ���� '=' ��� �������� �����
					{
						copyInArray(currentItem, tmpArray, instruction); //����������� ������� ������� �� ��������� ������
					}
				}
			}
		}
		if (strcmp(instruction, "=") == 0 && !readEqualSign) readEqualSign = true; //���� ������� - ���� '=' � ��� �� ���� ��������� ����� - �������� ��� �����������
		else if (strcmp(instruction, "=") != 0 && !readEqualSign) varBeforeEqualSign = var; //���� ������� - �� ���� '=' � ���� '=' �� ��� �������� ����� - ��������� ������� ��� ����������, ����������� ����� ������ '='

		instruction = strtok(NULL, " "); //������� ��������� �������
	}

	length = currentItem - 1; //���������� ����� ���������

	char *infix = new char[length + 2]; //������, � ������� �������� ��� ��������� � ��������� ������
	string *postfix = new string[length + 1]; //������, � ������� �������� ��������� � ����������� ������

	for (int i = 0; i < length; i++) //����������� ������� �� tmpArray � infix
	{
		infix[i] = tmpArray[i];
	}

	//�������� � ����� ������ '0' � ����������� ������ '\0'
	infix[length] = '0';
	infix[length + 1] = '\0';

	infixExpr.convertToPostfix(infix, length + 2, postfix, length + 1); //����������� �������������� ��������� � ��������� ������ � ��������� � ����������� ������

	var = (int)varBeforeEqualSign; // ��������� ���������� �� ����� '='

	postfixExpr.evaluatePostfixExpression(postfix, length + 1, symbolTable, TABLE_SIZE, instructionsSML, curInstrSML, varAddress); //������� SML-���������� �� ������������ ��������� ��� ���������� �� �����������

	instructionsSML[curInstrSML] = (21 * 100) + symbolTable[findSymbol(var,'V')].getLocation(); //������� � �������� SML-���������� �� �������� �������� �� ������������ � ������
	curInstrSML++; //������� �� ������ ������� � ������� SML-����������

	delete[] tmpArray; delete[] infix, delete[] postfix; //����������� ������ �� ��������, ��������� ����� ��������� ������������ ������
}

//����������� ������ � ���������� ������
void copyInArray(int &item, char* arr, char* str)
{
	for (int i = 0; i < strlen(str); i++) //������������ �����������
	{
		arr[item] = str[i];
		item++; //������� �� ��������� ������� �������
		arr[item] = ' '; //���������� ����������� �������
		item++; //������� �� ��������� ������� �������
	}
}

//�������� ������������ ������� ������ �� �������������� ����������
bool isOperator(char ch)
{
	return (ch == '=' || ch == '*' || ch == '/' || ch == '+' || ch == '-' || ch == '%' || ch == '^');
}

//�������� SML-����������, ��������������� ���������� � ���� �������
void functionExecute()
{
	bool wasRead = false; //����������, ��������� �� ���� �������

	int length = strlen(instruction);

	//������� �������� �������
	instruction = strtok(NULL, " ");

	char* func_name = new char[length]; //������� ������ ��� �������� ����� �������

	strcpy(func_name, instruction); //����������� ��� ������� � ������

	std::ios::pos_type pos = inputFile.tellg(); //��������� ������� ������� ������ �� �����

	inputFile.seekg(0); //������� � ������ �����

	inputFile.getline(buffer, BUFF_SIZE); //������� ������ �� �����

	instruction = strtok(buffer, " "); //������� ������ �������

	while (strcmp(instruction, func_name) != 0) //���� �� ������� ��������� ��� �������
	{
		instruction = strtok(NULL, " "); //������� ��������� �������

		if (instruction == NULL) //���� ��� ������ ���������
		{
			inputFile.getline(buffer, BUFF_SIZE); //������� ������ �� �����

			instruction = strtok(buffer, " "); //������� ������ �������
		}
	}

	//���������� ���� ������� � �������� SML-����������
	while (!wasRead)
	{
		inputFile.getline(buffer, BUFF_SIZE); //������� ������ �� �����

		strcpy(anchor, buffer); //��������� �������� ������

		instruction = strtok(buffer, " "); //������� ������ �������

		firstLexeme = true; //���� ��������� ������ �������

		while (instruction != NULL) //���� �� ��������� ��� ������
		{
			if (firstLexeme) //���� ������� - ������
			{
				strNum = atoi(instruction); //��������� ����� ������

				if ((rowWithValue = findSymbol(strNum, 'L')) == -1) //���� ����� ������ �� ������ � ������� ��������
				{
					symbolTable[currentRowTable].setSymbol(strNum);//��������� ����� ������
				}
				firstLexeme = false; //��������� ������� �� �������� ������
			}

			if (strcmp(instruction, "rem") == 0) //���������� ������� - "rem". ��������� ����� ������ ������������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������
				break; //����� �� ����� �� ���������� ������
			}

			else if (strcmp(instruction, "input") == 0) //���� ������. ����� input ���� ������ ����������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				instruction = strtok(NULL, " "); //������� ��������� �������

				while (instruction != NULL) //���� �� ������� ��� ������
				{
					var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
					{
						currentRowTable++; //������� �� ��������� ������ � �������
						varAddress--; //���������� ����� ����� � ������ ��� ����������

						//���������� ����� ���������� � ������� ������ � ��������� �����������
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						code = (10 * 100) + varAddress; //��������� SML-���������� ����� ����������

						instructionsSML[curInstrSML] = code; //�������� SML-���������� � ������

						curInstrSML++; //������� �� ������ ������� � ������� SML-����������
					}
					else //���� ������� � �������
					{
						code = (10 * 100) + symbolTable[rowWithValue].getLocation(); //��������� SML-���������� ����� ����������

						instructionsSML[curInstrSML] = code; //�������� SML-���������� � ������

						curInstrSML++; //������� �� ������ ������� � ������� SML-����������
					}

					instruction = strtok(NULL, " "); //������� ��������� �������
				}

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			else if (strcmp(instruction, "if") == 0) //�������� ��������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				instruction = strtok(NULL, " "); //������� ������ �������

				if (atoi(instruction) != 0 || isZero(instruction[0])) //������ ������� �������� ������
				{
					var = atoi(instruction); //��������� ����� � ����������

					if ((rowWithValue = findSymbol(var, 'C')) == -1) //���� ��������� �� ������� � ������� ��������
					{
						currentRowTable++; //������� �� ��������� ������ � �������
						varAddress--; //���������� ����� ����� � ������ ��� ���������

						//���������� ��������� � ������� ������ � ��������� �����������
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('C');
						symbolTable[currentRowTable].setLocation(varAddress);

						instructionsSML[varAddress] = var; //��������� �������� ��������� � ������� SML-����������

						firstOperand = varAddress; //��������� � ���������� ����� ������� �������� 
					}
					else firstOperand = symbolTable[rowWithValue].getLocation(); //���� ��������� ������� - ��������� � ���������� ����� ������� �������� 
				}
				else //������ ������� �������� ����������
				{
					var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
					{
						currentRowTable++; //������� �� ��������� ������ � �������
						varAddress--; //���������� ����� ����� � ������ ��� ����������

						//���������� ����� ���������� � ������� ������ � ��������� �����������
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						firstOperand = varAddress; //��������� � ���������� ����� ������� ��������
					}
					else firstOperand = symbolTable[rowWithValue].getLocation(); //���� ���������� ������� - ��������� � ���������� ����� ������� ��������
				}

				instruction = strtok(NULL, " "); //������� ��������� �������

				if (strcmp(instruction, "==") == 0) //���� ������� - "=="
				{
					equality(); //������� ������� equality
				}
				else if (strcmp(instruction, "!=") == 0) //���� ������� - "!="
				{
					notEqual(); //������� ������� notEqual
				}
				else if (strcmp(instruction, ">=") == 0) //���� ������� - ">="
				{
					greaterOrEqual(); //������� ������� greaterOrEqual
				}
				else if (strcmp(instruction, "<=") == 0) //���� ������� - "<="
				{
					lowerOrEqual(); //������� ������� lowerOrEqual
				}
				else if (strcmp(instruction, ">") == 0) //���� ������� - '>'
				{
					greater(); //������� ������� greater
				}
				else if (strcmp(instruction, "<") == 0) //���� ������� - '<'
				{
					lower(); //������� ������� lower
				}

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "let") == 0) //��������� ���������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				calculation(); //������� ������� calculation

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "goto") == 0) //������� �� ������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				instruction = strtok(NULL, " "); //������� ����� ��������. ������� �������� ������ ����� ������.

				var = atoi(instruction); //��������� ����� ������ � ����������

				if ((rowWithValue = findSymbol(var, 'L')) == -1) //���� ����� ������ �� ������ � �������, �� ����� ������ ��������� �������������. ����� ������� ���������� �������� ��� ������. �� ����� ������� ������� ���
					//������ ����� ����������������
				{
					flags[curInstrSML] = var; //��������� � ��������, ��������������� ������� SML-����������, ������� flags ����� ������ ��� �������� 

					code = (42 * 100); //������� ���������� �������� ��� ������

					instructionsSML[curInstrSML] = code; //�������� ����������

					curInstrSML++; //������� �� ������ ������� � ������� SML-����������
				}
				else //���� ����� ������ ������ � �������
				{
					code = (40 * 100) + symbolTable[rowWithValue].getLocation(); //������� ���������� �������� �� ������

					instructionsSML[curInstrSML] = code; //�������� ����������

					curInstrSML++; //������� �� ������ ������� � ������� SML-����������
				}

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "print") == 0) //����� ������ ��� �������� ���������� �� �����
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				instruction = strtok(NULL, " "); //������� �������

				while (instruction != NULL) //���� �� ��������� ��� ������
				{
					if (instruction[0] == '"') //���� ������ ������ ������� - ������� �������
					{
						char printString[100] = ""; //������� ���������� ������ ��� �������� ���� ������

						strcat(printString, instruction); //��������� ������� � �������
						instruction = strtok(NULL, ""); //��������� ��������� ������ �������
						if (instruction != NULL)
						{
							strcat(printString, " "); //������� ������ � ������
							strcat(printString, instruction); //��������� ������� � �������
						}

						int length = strlen(printString); //���������� ����� ������

						instructionsSML[curInstrSML] = (90 * 100) + (length - 2); //������� SML-���������� ��� ������� ����� ������ ��� ������� �������
						curInstrSML++; //������� �� ������ ������� � ������� SML-����������

						for (int i = 1; i < length; i++) //��������� ���������� ������ ��� ������� �������
						{
							instructionsSML[curInstrSML] = (90 * 100) + (int)printString[i];
							curInstrSML++;
						}
					}
					else //��������� ����������
					{
						var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

						if ((rowWithValue = findSymbol(var, 'V')) != -1) //���� ���������� ������� � ������� ��������
						{
							code = (11 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ������ �������� ����������

							instructionsSML[curInstrSML] = code; //�������� ����������

							curInstrSML++; //������� �� ������ ������� � ������� SML-����������
						}
						else if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
						{
							currentRowTable++; //������� �� ��������� ������ � �������
							varAddress--; //���������� ����� ����� � ������ ��� ����������

							//���������� ����� ���������� � ������� ������ � ��������� �����������
							symbolTable[currentRowTable].setSymbol(var);
							symbolTable[currentRowTable].setType('V');
							symbolTable[currentRowTable].setLocation(varAddress);

							code = (11 * 100) + varAddress; //������� SML-���������� ������ �������� ����������

							instructionsSML[curInstrSML] = code; //�������� ����������

							curInstrSML++; //������� �� ������ ������� � ������� SML-����������
						}
					}

					instruction = strtok(NULL, " "); //��������� ��������� �������
				}

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "for") == 0) //���� for
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				cycleFor(); //����� ������� cycleFor

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "gosub") == 0) //�������� �������� � �������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				functionExecute(); //����� ������� functionExecute

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			else if (strcmp(instruction, "return") == 0) wasRead = true; //���� �������� �������� "return" - ��������, ��� ���� ������� ���������

			instruction = strtok(NULL, " "); //��������� ��������� �������
		}

		if (currentRowTable > TABLE_SIZE) //���� ������� ������ ������� ������� �� ������������� �������
		{
			cerr << "��������� ������������ ������� ��������." << endl; //������ ������
			system("pause");
			system("exit"); //����� �� ���������
		}
		else
		{
			currentRowTable++; //������� � ��������� ������ � ������� �������� 
		}
	}

	inputFile.seekg(pos); //����� ���������� ���� ������� - ������� � ������ �����, ��������� �� ������� ������� ���������� "gosub"

	delete[] func_name; //����������� ������ �� ������������� ����������� �������
}

//�������� SML-����������, ��������������� ���������� � ���� ����� for
void cycleFor()
{
	bool wasRead = false; // //����������, ��������� �� ���� �����

	//parameter - ����� ����������-���������; cycleStart - ������ �����; rightSide - ����� �����; step - ��� �����
	int parameter = 0, cycleStart = -1, rightSide = 0, step = 0;

	instruction = strtok(NULL, " "); //������� �������

	var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

	if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
	{
		currentRowTable++; //������� �� ��������� ������ � �������
		varAddress--; //���������� ����� ����� � ������ ��� ����������

		//���������� ����� ���������� � ������� ������ � ��������� �����������
		symbolTable[currentRowTable].setSymbol(var);
		symbolTable[currentRowTable].setType('V');
		symbolTable[currentRowTable].setLocation(varAddress);

		parameter = varAddress; //��������� ����� ���������� 
	}
	else //���� ���������� ������� � ������� ��������
	{
		parameter = symbolTable[rowWithValue].getLocation(); //��������� ����� ���������� 
	}

	instruction = strtok(NULL, " "); //���������� '='
	instruction = strtok(NULL, " "); //��������� ��������� ��������

	if (atoi(instruction) != 0 || isZero(instruction[0])) //���� ������� - �����
	{
		var = atoi(instruction); //��������� ����� � ����������

		if ((rowWithValue = findSymbol(var, 'C')) == -1) //���� ��������� �� ������� � ������� ��������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ���������

			//���������� ����� ��������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = var; //�������� ��������� � SML-������
		}
	}
	else //���� ������� - ����������
	{
		var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

		if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ����������

			//���������� ����� ���������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('V');
			symbolTable[currentRowTable].setLocation(varAddress);
		}
	}

	instructionsSML[parameter] = atoi(instruction); //��������� �������� ������ ����� � SML-�������

	instruction = strtok(NULL, " "); //���������� "to"
	instruction = strtok(NULL, " "); //������� �������� ��������

	if (atoi(instruction) != 0 || isZero(instruction[0])) //���� ������� - �����
	{
		var = atoi(instruction); //��������� ����� � ����������

		if ((rowWithValue = findSymbol(var, 'C')) == -1) //���� ��������� �� ������� � ������� ��������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ���������

			//���������� ����� ��������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = var; //�������� ��������� � SML-������

			rightSide = varAddress; //��������� ����� �������� ����� ����� � SML-�������
		}
		else //���� ��������� ������� � ������� ��������
		{
			rightSide = symbolTable[rowWithValue].getLocation(); //��������� ����� �������� ����� ����� � SML-�������
		}
	}
	else //���� ������� - ����������
	{
		var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

		if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ����������

			//���������� ����� ���������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('V');
			symbolTable[currentRowTable].setLocation(varAddress);

			rightSide = varAddress; //��������� ����� �������� ����� ����� � SML-������
		}
		else  //���� ���������� ������� � ������� ��������
		{
			rightSide = symbolTable[rowWithValue].getLocation(); //��������� ����� �������� ����� ����� � SML-������
		}

	}

	instruction = strtok(NULL, " "); //����������, ������� �� �������� "step"

	if (instruction == NULL) //���� �������� "step" �����������
	{
		var = 1; //��������� ��� ����� � ����������

		if ((rowWithValue = findSymbol(var, 'C')) == -1) //���� ����� �� ������� � ������� ��������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ���������

			//���������� ����� ��������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = var; //�������� ��������� � SML-������
									
			step = varAddress; //��������� ����� �������� ���� ����� � SML-�������
		}
		else //���� ����� ������� � ������� ��������
		{
			step = symbolTable[rowWithValue].getLocation(); //��������� ����� �������� ���� ����� � SML-�������
		}
	}
	else //���� �������� "step" ������������
	{
		instruction = strtok(NULL, " "); //���������� ������ ���� �����

		var = atoi(instruction); //��������� ��� ����� � ����������

		if ((rowWithValue = findSymbol(var, 'C')) == -1) //���� ����� �� ������� � ������� ��������
		{
			currentRowTable++; //������� �� ��������� ������ � �������
			varAddress--; //���������� ����� ����� � ������ ��� ���������

			//���������� ����� ��������� � ������� ������ � ��������� �����������
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = var; //�������� ��������� � SML-������

			step = varAddress; //��������� ����� �������� ���� ����� � SML-�������
		}
		else
		{
			step = symbolTable[rowWithValue].getLocation(); //��������� ����� �������� ���� ����� � SML-�������
		}
	}

	//���������� ���������� ����� �� ��������� "next"
	while (!wasRead)
	{
		inputFile.getline(buffer, BUFF_SIZE); //������ ������ �� �����

		strcpy(anchor, buffer); //��������� �������� ������

		instruction = strtok(buffer, " "); //������� ������ �������

		firstLexeme = true; //���� ��������� ������ �������

		while (instruction != NULL) //���� �� ��������� ��� ������
		{
			if (firstLexeme) //���� ������� - ������
			{
				strNum = atoi(instruction); //��������� ����� ������

				if ((rowWithValue = findSymbol(strNum, 'L')) == -1) //���� ����� ������ �� ������ � ������� ��������
				{
					symbolTable[currentRowTable].setSymbol(strNum); //��������� ����� ������
				}

				if (cycleStart == -1) cycleStart = instructionAddress; //������ ������ �����

				firstLexeme = false; //��������� ������� �� �������� ������
			}

			if (strcmp(instruction, "rem") == 0) //���������� ������� - "rem". ��������� ����� ������ ������������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������
				break; //����� �� ����� �� ���������� ������
			}

			else if (strcmp(instruction, "input") == 0) //���� ������. ����� input ���� ������ ����������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				instruction = strtok(NULL, " "); //������� ��������� �������

				while (instruction != NULL) //���� �� ������� ��� ������
				{
					var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
					{
						currentRowTable++; //������� �� ��������� ������ � �������
						varAddress--; //���������� ����� ����� � ������ ��� ����������

						//���������� ����� ���������� � ������� ������ � ��������� �����������
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						code = (10 * 100) + varAddress; //��������� SML-���������� ����� ����������

						instructionsSML[curInstrSML] = code; //�������� SML-���������� � ������

						curInstrSML++; //������� �� ������ ������� � ������� SML-����������
					}
					else //���� ���������� ������� � ������� ��������
					{
						code = (10 * 100) + symbolTable[rowWithValue].getLocation(); //��������� SML-���������� ����� ����������

						instructionsSML[curInstrSML] = code; //�������� SML-���������� � ������

						curInstrSML++; //������� �� ������ ������� � ������� SML-����������
					}

					instruction = strtok(NULL, " "); //������� ��������� �������
				}

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			else if (strcmp(instruction, "if") == 0) //�������� ��������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				instruction = strtok(NULL, " "); //������� ������ �������

				if (atoi(instruction) != 0 || isZero(instruction[0])) //������ ������� �������� ������
				{
					var = atoi(instruction); //��������� ����� � ����������

					if ((rowWithValue = findSymbol(var, 'C')) == -1) //���� ��������� �� ������� � ������� ��������
					{
						currentRowTable++; //������� �� ��������� ������ � �������
						varAddress--; //���������� ����� ����� � ������ ��� ���������

						//���������� ��������� � ������� ������ � ��������� �����������
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('C');
						symbolTable[currentRowTable].setLocation(varAddress);

						instructionsSML[varAddress] = var; //��������� �������� ��������� � ������� SML-����������

						firstOperand = varAddress; //��������� � ���������� ����� ������� �������� 
					}
					else firstOperand = symbolTable[rowWithValue].getLocation(); //���� ��������� ������� - ��������� � ���������� ����� ������� ��������
				}
				else //������ ������� �������� ����������
				{
					var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
					{
						currentRowTable++; //������� �� ��������� ������ � �������
						varAddress--; //���������� ����� ����� � ������ ��� ����������

						//���������� ����� ���������� � ������� ������ � ��������� �����������
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						firstOperand = varAddress; //��������� � ���������� ����� ������� ��������
					}
					else firstOperand = symbolTable[rowWithValue].getLocation(); //���� ���������� ������� - ��������� � ���������� ����� ������� ��������
				}

				instruction = strtok(NULL, " "); //������� ��������� �������

				if (strcmp(instruction, "==") == 0) //���� ������� - "=="
				{
					equality(); //������� ������� equality
				}
				else if (strcmp(instruction, "!=") == 0) //���� ������� - "!="
				{
					notEqual(); //������� ������� notEqual
				}
				else if (strcmp(instruction, ">=") == 0) //���� ������� - ">="
				{
					greaterOrEqual(); //������� ������� greaterOrEqual
				}
				else if (strcmp(instruction, "<=") == 0) //���� ������� - "<="
				{
					lowerOrEqual(); //������� ������� lowerOrEqual
				}
				else if (strcmp(instruction, ">") == 0) //���� ������� - '>'
				{
					greater(); //������� ������� greater
				}
				else if (strcmp(instruction, "<") == 0) //���� ������� - '<'
				{
					lower(); //������� ������� lower
				}

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "let") == 0) //��������� ���������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				calculation(); //������� ������� calculation

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "goto") == 0) //������� �� ������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				instruction = strtok(NULL, " "); //������� ����� ��������. ������� �������� ������ ����� ������.

				var = atoi(instruction); //��������� ����� ������ � ����������

				if ((rowWithValue = findSymbol(var, 'L')) == -1) //���� ����� ������ �� ������ � �������, �� ����� ������ ��������� �������������. ����� ������� ���������� �������� ��� ������. �� ����� ������� ������� ���
					//������ ����� ����������������
				{
					flags[curInstrSML] = var; //��������� � ��������, ��������������� ������� SML-����������, ������� flags ����� ������ ��� �������� 

					code = (42 * 100); //������� ���������� �������� ��� ������

					instructionsSML[curInstrSML] = code; //�������� ����������

					curInstrSML++; //������� �� ������ ������� � ������� SML-����������
				}
				else //���� ����� ������ ������ � �������
				{
					code = (40 * 100) + symbolTable[rowWithValue].getLocation(); //������� ���������� �������� �� ������

					instructionsSML[curInstrSML] = code; //�������� ����������

					curInstrSML++; //������� �� ������ ������� � ������� SML-����������
				}

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "print") == 0) //����� ������ ��� �������� ���������� �� �����
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				instruction = strtok(NULL, " "); //������� �������

				while (instruction != NULL) //���� �� ��������� ��� ������
				{
					if (instruction[0] == '"') //���� ������ ������ ������� - ������� �������
					{
						char printString[100] = ""; //������� ���������� ������ ��� �������� ���� ������

						strcat(printString, instruction); //��������� ������� � �������
						instruction = strtok(NULL, ""); //��������� ��������� ������ �������
						if (instruction != NULL)
						{
							strcat(printString, " "); //������� ������ � ������
							strcat(printString, instruction); //��������� ������� � �������
						}

						int length = strlen(printString); //���������� ����� ������

						instructionsSML[curInstrSML] = (90 * 100) + (length - 2); //������� SML-���������� ��� ������� ����� ������ ��� ������� �������
						curInstrSML++; //������� �� ������ ������� � ������� SML-����������

						for (int i = 1; i < length; i++) //��������� ���������� ������ ��� ������� �������
						{
							instructionsSML[curInstrSML] = (90 * 100) + (int)printString[i];
							curInstrSML++;
						}
					}
					else //��������� ����������
					{
						var = tolower(instruction[0]); //��������� ASCII-��� �������� ����������

						if ((rowWithValue = findSymbol(var, 'V')) != -1) //���� ���������� ������� � ������� ��������
						{
							code = (11 * 100) + symbolTable[rowWithValue].getLocation(); //������� SML-���������� ������ �������� ����������

							instructionsSML[curInstrSML] = code; //�������� ����������

							curInstrSML++; //������� �� ������ ������� � ������� SML-����������
						}
						else if ((rowWithValue = findSymbol(var, 'V')) == -1) //���� ���������� �� ������� � ������� ��������
						{
							currentRowTable++; //������� �� ��������� ������ � �������
							varAddress--; //���������� ����� ����� � ������ ��� ����������

							//���������� ����� ���������� � ������� ������ � ��������� �����������
							symbolTable[currentRowTable].setSymbol(var);
							symbolTable[currentRowTable].setType('V');
							symbolTable[currentRowTable].setLocation(varAddress);

							code = (11 * 100) + varAddress; //������� SML-���������� ������ �������� ����������

							instructionsSML[curInstrSML] = code; //�������� ����������

							curInstrSML++; //������� �� ������ ������� � ������� SML-����������
						}
					}

					instruction = strtok(NULL, " "); //��������� ��������� �������
				}

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "gosub") == 0) //�������� �������� � �������
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				functionExecute(); //����� ������� functionExecute

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			/***************************************************************************/

			else if (strcmp(instruction, "for") == 0) //���� for
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				cycleFor(); //����� ������� cycleFor

				instructionAddress = curInstrSML; //��������� � ���������� ����� ����� ��������� SML-����������
			}

			else if (strcmp(instruction, "next") == 0)
			{
				symbolTable[currentRowTable].setType('L'); //��������� � ������� ������ ������� �������� ��� ������� (������)
				symbolTable[currentRowTable].setLocation(instructionAddress); //��������� � ������� ������ ������� �������� ����� ������, �� ������� ��������� ������

				wasRead = true; //���� �������� �������� "return" - ��������, ��� ���� ����� ���������
			}
			instruction = strtok(NULL, " "); //��������� ��������� �������
		}

		if (currentRowTable > TABLE_SIZE) //���� ������� ������ ������� ������� �� ������������� �������
		{
			cerr << "��������� ������������ ������� ��������." << endl; //������ ������
			system("pause");
			system("exit"); //����� �� ���������
		}
		else
		{
			currentRowTable++; //������� � ��������� ������ � ������� �������� 
		}
	}

	instructionsSML[curInstrSML] = (20 * 100) + parameter; //�������� SML-���������� �������� � ����������� �������� ���������
	curInstrSML++; //������� �� ������ ������� � ������� SML-����������

	instructionsSML[curInstrSML] = (30 * 100) + step; //�������� SML-���������� �������� ��������� � ���� �����
	curInstrSML++; //������� �� ������ ������� � ������� SML-����������

	instructionsSML[curInstrSML] = (21 * 100) + parameter; //�������� SML-���������� �������� �� ������������ �������� ���������
	curInstrSML++; //������� �� ������ ������� � ������� SML-����������



	instructionsSML[curInstrSML] = (20 * 100) + parameter; //�������� SML-���������� �������� � ����������� �������� ���������
	curInstrSML++; //������� �� ������ ������� � ������� SML-����������

	instructionsSML[curInstrSML] = (31 * 100) + rightSide; //�������� SML-���������� ��������� �������� ����� ������� ����� �� ������������ 
	curInstrSML++; //������� �� ������ ������� � ������� SML-����������

	instructionsSML[curInstrSML] = (41 * 100) + cycleStart; //�������� SML-���������� �������� �� ������
	curInstrSML++; //������� �� ������ ������� � ������� SML-����������

	instructionsSML[curInstrSML] = (42 * 100) + cycleStart; //�������� SML-���������� �������� �� ����
	curInstrSML++; //������� �� ������ ������� � ������� SML-����������
}

int checkSyntax()
{
	int numStr = 0; //���������� ����� ������, ����� ������� ��� � ��������� �� ������
	bool endWasFound = false; //�������� �� ���������� ���������� end � ��������� (end ����� ����������� ������ ���� ���).
	int countUnfinishedFor = 0; //���������� ���������� ������������� ������ FOR
	int strWhenSubBegin = 0; //��������� ������� ������ ���������� ������� � ����������, ����� ��������� return

	while (!inputFile.eof()) //���� �� ��������� ����� �����
	{
		numStr = 0; //������ ��������� �������� ��� ����������

		inputFile.getline(buffer, BUFF_SIZE); //������� ������ �� �����

		instruction = strtok(buffer, " "); //������� ������ �������

		firstLexeme = true; //���� ��������� ������ �������

		while (instruction != NULL) //���� �� ��������� ��� ������
		{
			if (firstLexeme) //���� ����������� ������� - ������
			{
				if (atoi(instruction) == 0) //������ ������� �� ����� ������
				{
					//������ ������ � ����� �� �������
					cerr << "�������� ����� ������ ������ \"" << instruction << "\"." << endl;
					system("pause");
					return 1;
				}
				else //������ ������� - ����� ������
				{
					firstLexeme = false; //��������� ����������� ������� �� �������� ������
					numStr = atoi(instruction); //��������� ����� ������
					instruction = strtok(NULL, " "); //��������� ��������� �������
					continue; //������ ��������� ��� �����
				}
			}

			if (strlen(instruction) == 2) //���� ����� ������� - ��� �������
			{
				if (strcmp(instruction, "if") == 0) //���� �������  - �������� "if"
				{
					instruction = strtok(NULL, " "); //���������� ������ ������������ ������� 

					if (instruction == NULL) //���� ������� �����������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": ��������� ��������� ����� ��������� \"if\"." << endl;
						system("pause");
						return 1;
					}
					else //���� ������� ������������
					{
						if (strlen(instruction) == 1) //���� ����� ������� - ���� ������
						{
							if (atoi(instruction) != 0) //������ ������� - ����� ������ 0
							{
								instruction = strtok(NULL, " "); //���������� ���������� ��������

								if (instruction == NULL) //���� ���������� �������� �����������
								{
									//������ ������ � ����� �� �������
									cerr << "������ " << numStr << ": ����������� ���������� ��������." << endl;
									system("pause");
									return 1;
								}
								else //���� ���������� �������� ������������
								{
									if (isLogical(instruction)) //���� ��� ���������� ��������
									{
										instruction = strtok(NULL, " "); //����������� ������� ��������

										if (instruction == NULL) //���� ������ ������� �����������
										{
											//������ ������ � ����� �� �������
											cerr << "������ " << numStr << ": ����������� ��������� ����� ����������� ���������." << endl;
											system("pause");
											return 1;
										}
										else if (strlen(instruction) == 1) //���� ������ ������� ������������ � ��� ����� - ���� ������
										{
											if (!isVar(instruction[0]) && !(instruction[0] >= 48 && instruction[0] <= 57)) //���� ������ �� ��������� � �� �����
											{
												//������ ������ � ����� �� �������
												cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ��� ������������� � ��������� \"if\"." << endl;
												system("pause");
												return 1;
											}
										}
										else if (strlen(instruction) > 1) //���� ����� ������� �������� ����� ������ �������
										{
											for (int i = 0; i < strlen(instruction); i++) //��������� ������ ������
											{
												if (!(instruction[i] >= 48 && instruction[i] <= 57)) //���� ������ �� ������������� �����
												{
													//������ ������ � ����� �� �������
													cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ��� ������������� � ��������� \"if\"." << endl;
													system("pause");
													return 1;
												}
											}
										}
									}
									else //��� �� ���������� ��������
									{
										//������ ������ � ����� �� �������
										cerr << "������ " << numStr << ": ����������� ���������� �������� \"" << instruction << "\"." << endl;
										system("pause");
										return 1;
									}
								}
							}
							else if (isVar(instruction[0]) || isZero(instruction[0])) //������ ������� - ���������� ��� ����� 0
							{
								instruction = strtok(NULL, " "); //���������� ���������� ��������

								if (instruction == NULL) //���� ���������� ������� �����������
								{
									//������ ������ � ����� �� �������
									cerr << "������ " << numStr << ": ����������� ���������� ��������." << endl;
									system("pause");
									return 1;
								}
								else //���� ���������� ������� ������������
								{

									if (isLogical(instruction)) //��� ���������� ��������
									{
										instruction = strtok(NULL, " "); //����������� ������� ��������

										if (instruction == NULL) //���� ������ ������� �����������
										{
											//������ ������ � ����� �� �������
											cerr << "������ " << numStr << ": ����������� ��������� ����� ����������� ���������." << endl;
											system("pause");
											return 1;
										}
										else if (strlen(instruction) == 1) //���� ������ ������� ������������ � ��� ����� - ���� ������
										{
											if (!isVar(instruction[0]) && !(instruction[0] >= 48 && instruction[0] <= 57)) //���� ������ �� ��������� � �� �����
											{
												//������ ������ � ����� �� �������
												cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ��� ������������� � ��������� \"if\"." << endl;
												system("pause");
												return 1;
											}
										}
										else if (strlen(instruction) > 1) //���� ����� ������� �������� ����� ������ �������
										{
											for (int i = 0; i < strlen(instruction); i++) //��������� ������ ������
											{
												if (!(instruction[i] >= 48 && instruction[i] <= 57)) //���� ������ �� ������������� �����
												{
													//������ ������ � ����� �� �������
													cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ��� ������������� � ��������� \"if\"." << endl;
													system("pause");
													return 1;
												}
											}
										}
									}
									else //��� �� ���������� ��������
									{
										//������ ������ � ����� �� �������
										cerr << "������ " << numStr << ": ����������� ���������� �������� \"" << instruction << "\"." << endl;
										system("pause");
										return 1;
									}
								}
							}
							else //���� ��������� ����������� ������
							{
								//������ ������ � ����� �� �������
								cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ��� ������������� � ��������� \"if\"." << endl;
								system("pause");
								return 1;
							}
						}
						else if (strlen(instruction) > 1) //���� ����� ������� ����� ������ �������
						{
							for (int i = 0; i < strlen(instruction); i++) //��������� ������ ������
							{
								if (!(instruction[i] >= 48 && instruction[i] <= 57)) //���� ������ �� ��������� � �� �����
								{
									//������ ������ � ����� �� �������
									cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ��� ������������� � ��������� \"if\"." << endl;
									system("pause");
									return 1;
								}
							}

							instruction = strtok(NULL, " "); //���������� ���������� ��������

							if (instruction == NULL) //���� ���������� ������� �����������
							{
								//������ ������ � ����� �� �������
								cerr << "������ " << numStr << ": ����������� ���������� ��������." << endl;
								system("pause");
								return 1;
							}
							else //���� ���������� ������� ������������
							{
								if (isLogical(instruction)) //��� ���������� ��������
								{
									instruction = strtok(NULL, " "); //����������� ������� ��������

									if (instruction == NULL) //���� ������ ������� �����������
									{
										//������ ������ � ����� �� �������
										cerr << "������ " << numStr << ": ����������� ��������� ����� ����������� ���������." << endl;
										system("pause");
										return 1;
									}
									else if (strlen(instruction) == 1) //���� ������ ������� ������������ � ��� ����� - ���� ������
									{
										if (!isVar(instruction[0]) && !(instruction[0] >= 48 && instruction[0] <= 57)) //���� ������ �� ��������� � �� �����
										{
											//������ ������ � ����� �� �������
											cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ��� ������������� � ��������� \"if\"." << endl;
											system("pause");
											return 1;
										}
									}
									else if (strlen(instruction) > 1) //���� ����� ������� �������� ����� ������ �������
									{
										for (int i = 0; i < strlen(instruction); i++) //��������� ������ ������
										{
											if (!(instruction[i] >= 48 && instruction[i] <= 57)) //���� ������ �� ������������� �����
											{
												//������ ������ � ����� �� �������
												cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ��� ������������� � ��������� \"if\"." << endl;
												system("pause");
												return 1;
											}
										}
									}
								}
								else //��� �� ���������� ��������
								{
									//������ ������ � ����� �� �������
									cerr << "������ " << numStr << ": ����������� ���������� �������� \"" << instruction << "\"." << endl;
									system("pause");
									return 1;
								}
							}
						}
						else //���� ��������� ����������� �������
						{
							//������ ������ � ����� �� �������
							cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ��� ������������� � ��������� \"if\"." << endl;
							system("pause");
							return 1;
						}
					}
					
					instruction = strtok(NULL, " "); //������� ��������� �������, ����� ��������� "goto"

					if (instruction == NULL) //���� �������� "goto" �����������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": ����������� �������� \"goto\" ����� ����������� ���������." << endl;
						system("pause");
						return 1;
					}
					else continue; //���� �������� "goto" ������������ - ������ ����� ��� ����� �� ����������� ������
				}
				else //���� ��������� ����������� �������
				{
					//������ ������ � ����� �� �������
					cerr << "������ " << numStr << ": ����������� �������� \"" << instruction << "\"." << endl;
					system("pause");
					return 1;
				}
			}
			else if (strlen(instruction) == 6) //���� ����� ������� - ����� ��������
			{
				if (strcmp(instruction, "return") == 0) //���� �������  - �������� "return"
				{
					strWhenSubBegin = 0; //�������� �������� ������, � ������� ������ ���������� �������
				}
				else //���� ��������� ����������� �������
				{
					//������ ������ � ����� �� �������
					cerr << "������ " << numStr << ": ����������� �������� \"" << instruction << "\"." << endl;
					system("pause");
					return 1;
				}
			}
			else if (strlen(instruction) == 5) //���� ����� ������� - ���� ��������
			{
				if (strcmp(instruction, "input") == 0) //���� �������  - �������� "input"
				{
					instruction = strtok(NULL, " "); //����������� ���������� 

					if (instruction == NULL) //���� ���������� �����������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": ����������� ���������� ����� ��������� \"input\"." << endl;
						system("pause");
						return 1;
					}
					else //���� ���������� ������������
					{
						while (instruction != NULL) //���� �� ��������� ��� ������
						{
							if (strlen(instruction) == 2 || strlen(instruction) == 1) //���� ������� ���������� ��� ���� �� ���������� (� �������)
							{
								if (!isVar(instruction[0])) //�� ������� ���������� ��� ����� ��������
								{
									//������ ������ � ����� �� �������
									cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ����������." << endl;
									system("pause");
									return 1;
								}
							}
							else //������� �� ������������� ������� ����������
							{
								//������ ������ � ����� �� �������
								cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ����������." << endl;
								system("pause");
								return 1;
							}

							instruction = strtok(NULL, " "); //������� ��������� �������
						}
					}
				}
				else if (strcmp(instruction, "print") == 0) //���� �������  - �������� "print"
				{
					instruction = strtok(NULL, " "); //����������� ���������� ��� ������

					if (instruction == NULL) //���� ���������� ��� ������ �����������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": ����������� ������ ��� ������ ����� ��������� \"print\"." << endl;
						system("pause");
						return 1;
					}
					else  //���� ���������� ��� ������ ������������
					{
						if (instruction[0] == '"') //���������� ������ ������
						{
							if (strlen(instruction) == 1) //������� �������� ������ ������ ������� �������
							{
								instruction = strtok(NULL, ""); //������� ��� ���������� ������

								if (instruction == NULL) //���� ����������� ����������� ������
								{
									//������ ������ � ����� �� �������
									cerr << "������ " << numStr << ": ����������� ����������� ������ \"." << endl;
									system("pause");
									return 1;
								}
							}
							else //������� �������� ����� ������ �������
							{
								if (instruction[0] == '"' && instruction[1] == '"') //����������� ������� � ������ ("")
								{
									//������ ������ � ����� �� �������
									cerr << "������ " << numStr << ": � ������ ����������� �������." << endl;
									system("pause");
									return 1;
								}

								if (!(instruction[0] == '"' && instruction[strlen(instruction) - 1] == '"')) //���� ������ �� ������� �� ������ �����
								{
									instruction = strtok(NULL, "");  //������� ��� ���������� ������

									if (instruction == NULL) //���� ����������� ����������� ������
									{
										//������ ������ � ����� �� �������
										cerr << "������ " << numStr << ": ����������� ����������� ������ \"." << endl;
										system("pause");
										return 1;
									}
									else if (instruction[strlen(instruction) - 1] != '"') //����������� ������ ������� �������
									{
										//������ ������ � ����� �� �������
										cerr << "������ " << numStr << ": ����������� ����������� ������ \"." << endl;
										system("pause");
										return 1;
									}
								}
							}
						}
						else //���������� ����������
						{
							while (instruction != NULL) //���� �� ��������� ��� ������
							{
								if (strlen(instruction) == 2 || strlen(instruction) == 1) //���� ������� ���������� ��� ���� �� ���������� (� �������)
								{
									if (!isVar(instruction[0])) //�� ������� ���������� ��� ������
									{
										//������ ������ � ����� �� �������
										cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ����������." << endl;
										system("pause");
										return 1;
									}
								}
								else //������� �� ������������� ������� ����������
								{
									//������ ������ � ����� �� �������
									cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ����������." << endl;
									system("pause");
									return 1;
								}

								instruction = strtok(NULL, " "); //������� ��������� �������
							}
						}

						instruction = strtok(NULL, " "); //������� ��������� �������
					}
				}
				else if (strcmp(instruction, "gosub") == 0) //���� �������  - �������� "gosub"
				{
					instruction = strtok(NULL, " "); //������� �������� �������

					if (instruction == NULL) //��� ������� �� �������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": ��������� ��� ������� ����� ��������� \"gosub\"." << endl;
						system("pause");
						return 1;
					}
					else //��������� ��������� ��� �������
					{
						if (atoi(instruction) != 0 || isZero(instruction[0])) //���� ��� ������� ���������� � �����
						{
							//������ ������ � ����� �� �������
							cerr << "������ " << numStr << ": ��� ������� �� ������ ���������� � ����." << endl;
							system("pause");
							return 1;
						}
						else
						{
							for (int i = 0; i < strlen(instruction); i++) //��������� ������ ������ � ����� �������
							{
								if (!(instruction[i] >= 65 && instruction[i] <= 90) && !(instruction[i] >= 97 && instruction[i] <= 122) && !(instruction[i] >= 48 && instruction[i] <= 57)) //���� ������ �� ������������� ����� ��� �����
								{
									//������ ������ � ����� �� �������
									cerr << "������ " << numStr << ": ��� ������� �������� ������� �������� �� \"a-z\", \"A-Z\" ��� \"0-9\"." << endl;
									system("pause");
									return 1;
								}
							}
						}

						//���� ������ � ����� ������� �����������

						char *func_name = new char[strlen(instruction)]; //������ ������ ��� �������

						strcpy(func_name, instruction); //��������� ��� �������

						if ((instruction = strtok(NULL, " ")) != NULL) //���� ����� ����� ������� �� �������� ����� ������
						{
							//������ ������ � ����� �� �������
							cerr << "������ " << numStr << ": ������ \"" << instruction << "\" �� ������ �������������� ����� ����� �������." << endl;
							system("pause");
							return 1;
						}

						std::ios::pos_type pos = inputFile.tellg(); //��������� ������� ������� ���������� �� �����

						inputFile.seekg(0); //������ ������ � ������ �����

						inputFile.getline(buffer, BUFF_SIZE); //������� ������ �� �����

						instruction = strtok(buffer, " "); //��������� ������ �������

						while (strcmp(instruction, func_name) != 0 && !inputFile.eof()) //���� �� ��������� ���������� ������ ������� ��� �� ��������� ����� �����
						{
							if (inputFile.tellg() == pos) //���� ������ �������� ������� ������ ������� - ������� � ��������� ������
							{
								inputFile.getline(buffer, BUFF_SIZE); //������� ������ �� �����

								instruction = strtok(buffer, " "); //��������� ������ �������

								continue; //����� ��� ���������� �����
							}

							instruction = strtok(NULL, " "); //������� �������

							if (instruction == NULL) //���� ��������� ��� ������
							{
								inputFile.getline(buffer, BUFF_SIZE); //������� ������ �� �����

								instruction = strtok(buffer, " "); //��������� ������ �������
							}
						}

						if (inputFile.eof()) //���� ������� �� ���� �������
						{
							//������ ������ � ����� �� �������
							cerr << "������� \"" << func_name << "\" �� �������." << endl;
							system("pause");
							return 1;
						}

						inputFile.seekg(pos); //��������� � ��������� ������� ��� ����������� ���������� �����

						delete[] func_name; //����������� ������
					}
				}
				else //���� ��������� ����������� �������
				{
					//������ ������ � ����� �� �������
					cerr << "������ " << numStr << ": ����������� �������� \"" << instruction << "\"." << endl;
					system("pause");
					return 1;
				}
			}
			else if (strlen(instruction) == 4) //���� ����� ������� - ������ �������
			{
				if (strcmp(instruction, "goto") == 0) //���� �������  - �������� "goto"
				{
					instruction = strtok(NULL, " "); //���������� ����� ��������

					if (instruction == NULL) //���� ����� �������� �����������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": ����������� ����� �������� ����� ��������� \"goto\"." << endl;
						system("pause");
						return 1;
					}

					if (atoi(instruction) == 0) //���� ����� �������� �� �������� ������� ������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": ������� �������� ����� ������� ������ ����� ������." << endl;
						system("pause");
						return 1;
					}

					int length = strlen(instruction) + 1;

					char *branch_address = new char[length]; //������ ������ ����� ��������

					strcpy(branch_address, instruction); //��������� ����� ��������

					std::ios::pos_type pos = inputFile.tellg(); //��������� ������� ������� ���������� �� �����

					inputFile.seekg(0); //������ ������ � ������ �����
					
					if ((instruction = strtok(NULL, " ")) != NULL) //���� ����� ������ �������� �� �������� ����� ������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": ��������� ���������� ������� �������� ����� ��������� \"goto\"." << endl;
						system("pause");
						delete[] branch_address; //����������� ������
						return 1;
					}

					inputFile.getline(buffer, BUFF_SIZE); //������� ������ �� �����
					 
					instruction = strtok(buffer, " "); //��������� ������ �������

					while (strcmp(branch_address, instruction) != 0 && !inputFile.eof()) //���� �� ��������� ������ �������� ��� �� ��������� ����� �����
					{
						inputFile.getline(buffer, BUFF_SIZE); //������� ������ �� �����

						instruction = strtok(buffer, " "); //��������� ������ �������
					}

					if (inputFile.eof()) //���� ������ �� ���� �������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": ����� ������, ��������� ����� ��������� \"goto\" �� ������." << endl;
						system("pause");
						delete[] branch_address; //����������� ������
						return 1;
					}

					inputFile.seekg(pos); //��������� � ��������� ������� ��� ����������� ���������� �����

					delete[] branch_address; //����������� ������

					break; //����� �� ����� �� ���������� ������
				}
				else if (strcmp(instruction, "next") == 0) //���� �������  - �������� "next"
				{
					countUnfinishedFor--; //��������� ����������� ���������� ������ "for" 

					if (countUnfinishedFor < 0) //���� ���������� ���������� ������ "for" ������ ����
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": ����������� ���� \"for\" ����� ���������� \"next\"." << endl;
						system("pause");
						return 1;
					}
				}
				else //���� ��������� ����������� �������
				{
					//������ ������ � ����� �� �������
					cerr << "������ " << numStr << ": ����������� �������� \"" << instruction << "\"." << endl;
					system("pause");
					return 1;
				}
			}
			else if (strlen(instruction) == 3) //���� ����� ������� - ��� �������
			{
				if (strcmp(instruction, "rem") == 0) //���� �������  - �������� "rem"
				{
					break; //������� �� ��������� ������
				}
				else if (strcmp(instruction, "let") == 0) //���� �������  - �������� "let"
				{
					instruction = strtok(NULL, " "); //������� ����������, �������� ��������� ���������

					if (instruction == NULL) //���� ���������� �����������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": ��������� ��������� ����� ��������� \"let\"." << endl;
						system("pause");
						return 1;
					}
					
					if (strlen(instruction) == 1) //���� ����� ������� - ���� ������
					{
						if (isVar(instruction[0])) //���� ������ - ����������
						{
							instruction = strtok(NULL, " "); //������� ���� '='

							if (instruction == NULL) //���� ������� �����
							{
								//������ ������ � ����� �� �������
								cerr << "������ " << numStr << ": ���������� �� ��������� ���������." << endl;
								system("pause");
								return 1;
							}
							else //���� ������� �� �����
							{
								if (strlen(instruction) == 1) //���� ����� ������� - ���� ������
								{
									if (instruction[0] == '=') //���� ������ - ���� '='
									{
										instruction = strtok(NULL, " "); //������� ������ ������� �� ���������

										if (instruction == NULL) //���� ������� �����
										{
											//������ ������ � ����� �� �������
											cerr << "������ " << numStr << ": ��������� ��������� ����� \"=\"." << endl;
											system("pause");
											return 1;
										}
										else //���� ������� �� ����� - ���������� ���������� ���������
										{
											int couplesOfbrackets = 0; //���������, �������� �� � ��������� ���������� ������
											bool hasFirstOperand = false; //���������, ������ �� ������ ������� ����� ����������� �������������� ������ ��� ����������
											bool hasOperator = false; //���������, ������ �� �������������� ��������
											
											while (instruction != NULL) //���� �� ��������� ��� ������
											{
												if (strlen(instruction) == 1) //���� ����� ������� - ���� ������
												{
													if (isVar(instruction[0])) //������� �������� ����������
													{
														if (hasFirstOperand == false && hasOperator == false) //���������� �������� ������ ��������� � ���������
														{
															hasFirstOperand = true; //�������, ��� ��������������� ������ �������
														}
														else if (hasFirstOperand == true && hasOperator == true) //���������� �������� ������ ��������� � ���������
														{
															hasOperator = false; //��������� ����������. ���������� �������� false ��� hasOperator, ����� ������� ������� ���� ������ � ��������� ��������� �������������� ��������� 
														}
														else if (hasFirstOperand == true && hasOperator == false) //���������� ������� �� ������ ���������
														{
															//������ ������ � ����� �� �������
															cerr << "������ " << numStr << ": ��������� �������������� �������� ����� \'" << instruction[0] << "\'." << endl;
															system("pause");
															return 1;
														}

														instruction = strtok(NULL, " "); //��������� ��������� �������
														continue; //������ ����� ��� �����
													}
													else if (atoi(instruction) != 0 || isZero(instruction[0])) //������� �������� ����������� ������
													{
														if (hasFirstOperand == false && hasOperator == false) //����� �������� ������ ��������� � ���������
														{
															hasFirstOperand = true; //�������, ��� ��������������� ������ �������
														}
														else if (hasFirstOperand == true && hasOperator == true) //����� �������� ������ ��������� � ���������
														{
															hasOperator = false; //��������� ����������. ���������� �������� false ��� hasOperator, ����� ������� ������� ���� ������ � ��������� ��������� �������������� ���������
														}
														else if (hasFirstOperand == true && hasOperator == false) //����� ������� �� ������ ���������
														{
															//������ ������ � ����� �� �������
															cerr << "������ " << numStr << ": ��������� �������������� �������� ����� \'" << instruction[0] << "\'." << endl;
															system("pause");
															return 1;
														}

														instruction = strtok(NULL, " "); //��������� ��������� �������
														continue; //������ ����� ��� �����
													}
													else if (isOperator(instruction[0]) && instruction[0] != '=') //������� �������� �������������� ����������
													{
														if (hasFirstOperand == false) //���� ����������� ������ ������� ����� �������������� ����������
														{
															//������ ������ � ����� �� �������
															cerr << "������ " << numStr << ": ��������� ������� ����� \'" << instruction[0] << "\'." << endl;
															system("pause");
															return 1;
														}
														else if (hasOperator == true) //�������� ������� �� ������ ����������
														{
															//������ ������ � ����� �� �������
															cerr << "������ " << numStr << ": \'" << instruction[0] << "\' �� �������� ���������." << endl;
															system("pause");
															return 1;
														}

														instruction = strtok(NULL, " "); //������� ��������� �������

														if (instruction == NULL) //����������� ������ �������
														{
															//������ ������ � ����� �� �������
															cerr << "������ " << numStr << ": ����������� ������� ����� ��������������� ���������." << endl;
															system("pause");
															return 1;
														}
														else //������ ��������� ������ �������
														{
															hasOperator = true; //��������� �������������� �������� � ���������
														}
													}
													else //������ �� ����� �������������� � �������������� ���������
													{
														//������ ������ � ����� �� �������
														cerr << "������ " << numStr << ": ������ \'" << instruction[0] << "\' �� �������� ��� ������������� � ���������." << endl;
														system("pause");
														return 1;
													}
												}
												else //���� ����� ������� ������ ������ �������
												{
													int length = strlen(instruction); //��������� ����� �������

													if (instruction[0] == '(') //������� ���� ����������� ������ ���� "(x"
													{
														couplesOfbrackets++; //��������� ���������� ���������� ������

														if (instruction[length - 1] == ')') //������ ������ ��������� ������ ����� ��� ���������� ���� "(x)"
														{
															couplesOfbrackets--; //��������� ���������� ���������� ������

															if (length == 2) //���� ������ ������
															{
																//������ ������ � ����� �� �������
																cerr << "������ " << numStr << ": ����������� ��������� ����� \'(\' � \')\'." << endl;
																system("pause");
																return 1;
															}
															else //���� ������ �� ������ - ����������� ����������
															{
																char *content = new char[length - 2]; //������ ������ ���������� ������ ������

																for (int i = 1; i < length - 1; i++) //����������� ������� � ������
																{
																	content[i - 1] = instruction[i];
																}

																bool nextIsDuplicateSign = false; //����������, ������� �� ����������� �������������� �������� ���� "++"
																bool nextIsDuplicateVar = false; //����������, ������� �� ������������� ���������� ���� "xx" ��� "xz"
																bool varWasDefined = false; //����������, ���� �� ��������� ���������� � ���������
																bool digitWasDefined = false; //����������, ���� �� ��������� ����� � ���������

																for (int i = 0; i < strlen(content); i++) //��������� ������ ������
																{
																	if (content[i] == '-' || content[i] == '+') //���� ������ �������� ������ '+' ��� '-', ������������ ��������������� �����
																	{
																		if (nextIsDuplicateSign) //���� ������ ������ ����������� ��� ���
																		{
																			//������ ������ � ����� �� �������
																			cerr << "������ " << numStr << ": �������� ������������� ������ \'" << content[i] << "\' ����� \'(\' � \')\'." << endl;
																			system("pause");
																			delete[] content; //����������� ������
																			return 1;
																		}
																		else if (digitWasDefined) //���� ����� ������ ������� ����� ���� "1+"
																		{
																			//������ ������ � ����� �� �������
																			cerr << "������ " << numStr << ": ������������ �������� \"" << content << "\"." << endl;
																			system("pause");
																			delete[] content; //����������� ������
																			return 1;
																		}
																		else
																		{
																			nextIsDuplicateSign = true; //��������� ����� �� ������ ������� ������
																		}
																	}
																	else if (isVar(content[i])) //���� ������ �������� ����������
																	{
																		if (nextIsDuplicateVar) //������������� ��� ���������� ���� "xx" ��� "xz"
																		{
																			//������ ������ � ����� �� �������
																			cerr << "������ " << numStr << ": ��������� ������������� ���������� \'" << content[i] << "\' ����� \'(\' � \')\'." << endl;
																			system("pause");
																			delete[] content; //����������� ������
																			return 1;
																		}
																		else if (digitWasDefined) //�� ���������� ���� ����� ���� "1y"
																		{
																			//������ ������ � ����� �� �������
																			cerr << "������ " << numStr << ": ������������ ��� ���������� \"" << content[i - 1] << content[i] << "\"." << endl;
																			system("pause");
																			delete[] content; //����������� ������
																			return 1;
																		}
																		else
																		{
																			varWasDefined = true; //���������� ���������
																			nextIsDuplicateVar = true; //��������� ������ �� ������ ���� ����������
																		}
																	}
																	else if (content[i] >= 48 && content[i] <= 57) //���� ������ �������� ������
																	{
																		if (varWasDefined) //����� ���������� ���� ����� ���� "x2"
																		{
																			//������ ������ � ����� �� �������
																			cerr << "������ " << numStr << ": ������������ ��� ���������� \"" << content[i - 1] << content[i] << "\"." << endl;
																			system("pause");
																			delete[] content; //����������� ������
																			return 1;
																		}
																		else
																		{
																			digitWasDefined = true; //�������, ��� ���� ��������� �����
																		}
																	}
																	else //���� ������ ����������
																	{
																		//������ ������ � ����� �� �������
																		cerr << "������ " << numStr << ": ������������� ������ \'" << content[i] << "\'." << endl;
																		system("pause");
																		delete[] content; //����������� ������
																		return 1;
																	}
																}

																delete[] content; //����������� ������
															}
														}
														else //������ ������ - ���������
														{
															/*----------------- �������� ������������ ����������� � '(' � ������ -------------*/

															bool nextIsDuplicateSign = false; //����������, ������� �� ����������� �������������� �������� ���� "++"
															bool nextIsDuplicateVar = false; //����������, ������� �� ������������� ���������� ���� "xx" ��� "xz"
															bool varWasDefined = false; //����������, ���� �� ��������� ���������� � ���������
															bool digitWasDefined = false; //����������, ���� �� ��������� ����� � ���������

															for (int i = 1; i < length; i++) //��������� ������ ������ ����� '(' � ������
															{
																if (instruction[i] == '-' || instruction[i] == '+') //���� ������ �������� ������ '+' ��� '-', ������������ ��������������� �����
																{
																	if (nextIsDuplicateSign) //���� ������ ������ ����������� ��� ���
																	{
																		//������ ������ � ����� �� �������
																		cerr << "������ " << numStr << ": �������� ������������� ������ \'" << instruction[i] << "\' ����� \'(\' � \')\'." << endl;
																		system("pause");
																		return 1;
																	}
																	else if (digitWasDefined) //���� ����� ������ ������� ����� ���� "1+"
																	{
																		//������ ������ � ����� �� �������
																		cerr << "������ " << numStr << ": ������������ �������� \"" << instruction << "\"." << endl;
																		system("pause");
																		return 1;
																	}
																	else
																	{
																		nextIsDuplicateSign = true; //��������� ����� �� ������ ������� ������
																	}
																}
																else if (isVar(instruction[i])) //���� ������ �������� ����������
																{
																	if (nextIsDuplicateVar) //������������� ��� ���������� ���� "xx" ��� "xz"
																	{
																		//������ ������ � ����� �� �������
																		cerr << "������ " << numStr << ": ��������� ������������� ���������� \'" << instruction[i] << "\' ����� \'(\' � \')\'." << endl;
																		system("pause");
																		return 1;
																	}
																	else if (digitWasDefined) //�� ���������� ���� ����� ���� "1y"
																	{
																		//������ ������ � ����� �� �������
																		cerr << "������ " << numStr << ": ������������ ��� ���������� \"" << instruction[i - 1] << instruction[i] << "\"." << endl;
																		system("pause");
																		return 1;
																	}
																	else
																	{
																		varWasDefined = true; //���������� ���������
																		nextIsDuplicateVar = true; //��������� ������ �� ������ ���� ����������
																	}
																}
																else if (instruction[i] >= 48 && instruction[i] <= 57) //���� ������ �������� ������
																{
																	if (varWasDefined) //����� ���������� ���� ����� ���� "x2"
																	{
																		//������ ������ � ����� �� �������
																		cerr << "������ " << numStr << ": ������������ ��� ���������� \"" << instruction[i - 1] << instruction[i] << "\"." << endl;
																		system("pause");
																		return 1;
																	}
																	else
																	{
																		digitWasDefined = true; //�������, ��� ���� ��������� �����
																	}
																}
																else //���� ������ ����������
																{
																	//������ ������ � ����� �� �������
																	cerr << "������ " << numStr << ": ������������� ������ \'" << instruction[i] << "\'." << endl;
																	system("pause");
																	return 1;
																}
															}

															/*------------------ ��������� �������� ------------------------*/

															hasFirstOperand = true; //�������, ��� ��������������� ������ �������
															hasOperator = false; //��������� ����������. ���������� �������� false ��� hasOperator, ����� ������� ������� ���� ������ � ��������� ��������� �������������� ���������

															instruction = strtok(NULL, " "); //���������� ������ ���������
															continue; //������ ����� ��� �����
														}
													}
													else if (instruction[length - 1] == ')') //������� ����������� ������ ���� "x)"
													{
														couplesOfbrackets--; //��������� ���������� ���������� ������

														/*----------------- �������� ������������ ����������� � ')' � ����� -------------*/

														bool nextIsDuplicateSign = false; //����������, ������� �� ����������� �������������� �������� ���� "++"
														bool nextIsDuplicateVar = false; //����������, ������� �� ������������� ���������� ���� "xx" ��� "xz"
														bool varWasDefined = false; //����������, ���� �� ��������� ���������� � ���������
														bool digitWasDefined = false; //����������, ���� �� ��������� ����� � ���������

														for (int i = 0; i < length - 1; i++)
														{
															if (instruction[i] == '-' || instruction[i] == '+') //��������� ������ ������
															{
																if (nextIsDuplicateSign) //���� ������ ������ ����������� ��� ��� 
																{
																	//������ ������ � ����� �� �������
																	cerr << "������ " << numStr << ": �������� ������������� ������ \'" << instruction[i] << "\' ����� \'(\' � \')\'." << endl;
																	system("pause");
																	return 1;
																}
																else if (digitWasDefined) //���� ����� ������ ������� ����� ���� "1+"
																{
																	//������ ������ � ����� �� �������
																	cerr << "������ " << numStr << ": ������������ �������� \"" << instruction << "\"." << endl;
																	system("pause");
																	return 1;
																}
																else
																{
																	nextIsDuplicateSign = true; //��������� ����� �� ������ ������� ������
																}
															}
															else if (isVar(instruction[i])) //���� ������ �������� ����������
															{
																if (nextIsDuplicateVar) //������������� ��� ���������� ���� "xx" ��� "xz"
																{
																	//������ ������ � ����� �� �������
																	cerr << "������ " << numStr << ": ��������� ������������� ���������� \'" << instruction[i] << "\' ����� \'(\' � \')\'." << endl;
																	system("pause");
																	return 1;
																}
																else if (digitWasDefined) //�� ���������� ���� ����� ���� "1y"
																{
																	//������ ������ � ����� �� �������
																	cerr << "������ " << numStr << ": ������������ ��� ���������� \"" << instruction[i - 1] << instruction[i] << "\"." << endl;
																	system("pause");
																	return 1;
																}
																else
																{
																	varWasDefined = true; //���������� ���������
																	nextIsDuplicateVar = true; //��������� ������ �� ������ ���� ����������
																}
															}
															else if (instruction[i] >= 48 && instruction[i] <= 57) //���� ������ �������� ������
															{
																if (varWasDefined) //����� ���������� ���� ����� ���� "x2"
																{
																	//������ ������ � ����� �� �������
																	cerr << "������ " << numStr << ": ������������ ��� ���������� \"" << instruction[i - 1] << instruction[i] << "\"." << endl;
																	system("pause");
																	return 1;
																}
																else
																{
																	digitWasDefined = true; //�������, ��� ���� ��������� �����
																}
															}
															else //���� ������ ����������
															{
																//������ ������ � ����� �� �������
																cerr << "������ " << numStr << ": ������������� ������ \'" << instruction[i] << "\'." << endl;
																system("pause");
																return 1;
															}
														}

														/*------------------ ��������� �������� ------------------------*/
													}
													else //������� ��������� ������
													{
														/*----------------- �������� ������������ ����������� -------------*/

														bool nextIsDuplicateSign = false; //����������, ������� �� ����������� �������������� �������� ���� "++"
														bool nextIsDuplicateVar = false; //����������, ������� �� ������������� ���������� ���� "xx" ��� "xz"
														bool varWasDefined = false; //����������, ���� �� ��������� ���������� � ���������
														bool digitWasDefined = false; //����������, ���� �� ��������� ����� � ���������

														for (int i = 0; i < strlen(instruction); i++)
														{
															if (instruction[i] == '-' || instruction[i] == '+') //��������� ������ ������
															{
																if (nextIsDuplicateSign) //���� ������ ������ ����������� ��� ��� 
																{
																	//������ ������ � ����� �� �������
																	cerr << "������ " << numStr << ": �������� ������������� ������ \'" << instruction[i] << "\' ����� \'(\' � \')\'." << endl;
																	system("pause");
																	return 1;
																}
																else if (digitWasDefined) //���� ����� ������ ������� ����� ���� "1+"
																{
																	//������ ������ � ����� �� �������
																	cerr << "������ " << numStr << ": ������������ �������� \"" << instruction << "\"." << endl;
																	system("pause");
																	return 1;
																}
																else
																{
																	nextIsDuplicateSign = true; //��������� ����� �� ������ ������� ������
																}
															}
															else if (isVar(instruction[i])) //���� ������ �������� ����������
															{
																if (nextIsDuplicateVar)  //������������� ��� ���������� ���� "xx" ��� "xz"
																{
																	//������ ������ � ����� �� �������
																	cerr << "������ " << numStr << ": ��������� ������������� ���������� \'" << instruction[i] << "\' ����� \'(\' � \')\'." << endl;
																	system("pause");
																	return 1;
																}
																else if (digitWasDefined) //�� ���������� ���� ����� ���� "1y"
																{
																	//������ ������ � ����� �� �������
																	cerr << "������ " << numStr << ": ������������ ��� ���������� \"" << instruction[i - 1] << instruction[i] << "\"." << endl;
																	system("pause");
																	return 1;
																}
																else
																{
																	varWasDefined = true; //���������� ���������
																	nextIsDuplicateVar = true; //��������� ������ �� ������ ���� ����������
																}
															}
															else if (instruction[i] >= 48 && instruction[i] <= 57) //���� ������ �������� ������
															{
																if (varWasDefined) //����� ���������� ���� ����� ���� "x2"
																{
																	//������ ������ � ����� �� �������
																	cerr << "������ " << numStr << ": ������������ ��� ���������� \"" << instruction[i - 1] << instruction[i] << "\"." << endl;
																	system("pause");
																	return 1;
																}
																else
																{
																	digitWasDefined = true; //�������, ��� ���� ��������� �����
																}
															}
															else //���� ������ ����������
															{
																//������ ������ � ����� �� �������
																cerr << "������ " << numStr << ": ������������� ������ \'" << instruction[i] << "\'." << endl;
																system("pause");
																return 1;
															}
														}

														/*------------------ ��������� �������� ------------------------*/
													}

													hasFirstOperand = true; //�������, ��� ��������������� ������ �������
													hasOperator = false; //��������� ����������. ���������� �������� false ��� hasOperator, ����� ������� ������� ���� ������ � ��������� ��������� �������������� ���������

													instruction = strtok(NULL, " "); //���������� ������ ���������
													continue; //������ ����� ��� �����;
												}
											}

											if (couplesOfbrackets > 0) //���� ���������� ���������� ������ ������ ����
											{
												//������ ������ � ����� �� �������
												cerr << "������ " << numStr << ": ����������� ����������� ������ � ���������." << endl;
												system("pause");
												return 1;
											}
											else if (couplesOfbrackets < 0) //���� ���������� ���������� ������ ������ ����
											{
												//������ ������ � ����� �� �������
												cerr << "������ " << numStr << ": ������� ������ ����������� ������ � ���������." << endl;
												system("pause");
												return 1;
											}
										}
									}
									else //���� ������ �� �������� ������ '='
									{
										//������ ������ � ����� �� �������
										cerr << "������ " << numStr << ": �������� \"=\" ������ \"" << instruction << "\"." << endl;
										system("pause");
										return 1;
									}
								}
								else //���� ����� ������� ����� ������ �������
								{
									//������ ������ � ����� �� �������
									cerr << "������ " << numStr << ": �������� \"=\" ������ \"" << instruction << "\"." << endl;
									system("pause");
									return 1;
								}
							}
						}
						else //���� ������ �� ����������
						{
							//������ ������ � ����� �� �������
							cerr << "������ " << numStr << ": ��������� ���������� ������ \"" << instruction << "\"." << endl;
							system("pause");
							return 1;
						}
					}
					else //���� ����� ������� ����� ������ �������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": ��������� ���������� ������ \"" << instruction << "\"." << endl;
						system("pause");
						return 1;
					}
				}
				else if (strcmp(instruction, "end") == 0) //���� �������  - �������� "end"
				{
					if (endWasFound == false) //�������� ������ �������� end
					{
						endWasFound = true; //�������, ��� �������� "end" ��������������� � ���������
					}
					else //�������� ��� ���� �������� end
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": �������� \"end\" ����� ���� �������� ������ ���� ��� � ���������." << endl;
						system("pause");
						return 1;
					}

					if ((instruction = strtok(NULL, " ")) != NULL) //���� ����� ��������� end �� �������� ����� ������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": �������� \"end\" �� ������ ��������� ������� ����� ������ ����������." << endl;
						system("pause");
						return 1;
					}

					if (countUnfinishedFor > 0) //���� ���������� �������� ������ "for" ������ 0
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": �������� �������� \"next\" ������ ��������� \"end\"." << endl;
						system("pause");
						return 1;
					}

					if (strWhenSubBegin != 0) //���� � ���������� �������� ����� ������ ���������� �������, ������� �� ���� ������� ���������� "return"
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": �������� �������� \"return\" ������ ��������� \"end\"." << endl;
						system("pause");
						return 1;
					}

					break; //����� �� ����� �� ���������� ������
				}
				else if (strcmp(instruction, "sub") == 0) //���� �������  - �������� "sub"
				{
					instruction = strtok(NULL, " "); //������� �������� �������

					if (instruction == NULL) //��� ������� �� �������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": ��������� ��� ������� ����� ��������� \"sub\"." << endl;
						system("pause");
						return 1;
					}
					else //��������� ��������� ��� �������
					{
						if (strWhenSubBegin == 0) //���� ����� ������ ��� �� �������� � ����������
						{
							strWhenSubBegin = numStr; //��������� ����� ������
						}
						else //���� � ��������� ������� ������ ����� ������
						{
							//������ ������ � ����� �� �������
							cerr << "������ " << numStr << ": ��������� �������� \"return\"." << endl;
							system("pause");
							return 1;
						}

						if (atoi(instruction) != 0 || isZero(instruction[0])) //���� ��� ������� ���������� � ����
						{
							//������ ������ � ����� �� �������
							cerr << "������ " << numStr << ": ��� ������� �� ������ ���������� � ����." << endl;
							system("pause");
							return 1;
						}
						else
						{
							for (int i = 0; i < strlen(instruction); i++) //��������� ������ ������
							{
								//���� ������ �� �������� ������ ��� ��������� ������
								if (!(instruction[i] >= 65 && instruction[i] <= 90) && !(instruction[i] >= 97 && instruction[i] <= 122) && !(instruction[i] >= 48 && instruction[i] <= 57))
								{
									//������ ������ � ����� �� �������
									cerr << "������ " << numStr << ": ��� ������� �������� ������� �������� �� \"a-z\", \"A-Z\" ��� \"0-9\"." << endl;
									system("pause");
									return 1;
								}
							}
						}
					}

					if ((instruction = strtok(NULL, " ")) != NULL) //���� ����� ����� ������� �� �������� ����� ������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": ������ \"" << instruction << "\" �� ������ �������������� ����� ����� �������." << endl;
						system("pause");
						return 1;
					}
				}
				else if (strcmp(instruction, "for") == 0) //���� �������  - �������� "for"
				{
					instruction = strtok(NULL, " "); //������� ��������

					if (instruction == NULL) //���� �������� �����������
					{
						//������ ������ � ����� �� �������
						cerr << "������ " << numStr << ": �� ������ �������� ����� ��������� \"for\"." << endl;
						system("pause");
						return 1;
					}
					else //���� �������� ������������
					{
						countUnfinishedFor++; //��������� ���������� ������������� ������ "for"

						if (strlen(instruction) == 1) //���� ����� ������� - ���� ������
						{
							if (isVar(instruction[0])) //���� ������� - ����������
							{
								instruction = strtok(NULL, " "); //������� ���� '='

								if (instruction == NULL) //���� ������� ����������
								{
									//������ ������ � ����� �� �������
									cerr << "������ " << numStr << ": ����������� ������������� ��������� � ����� \"for\"." << endl;
									system("pause");
									return 1;
								}
								else //���� ������� ������������
								{
									if (instruction[0] == '=' && strlen(instruction) == 1) //���� ������� �������� ������ '='
									{
										instruction = strtok(NULL, " "); //������� ��������� ��������

										if (instruction == NULL) //���� ��������� �������� �����������
										{
											//������ ������ � ����� �� �������
											cerr << "������ " << numStr << ": ����������� ������������� ��������� � ����� \"for\"." << endl;
											system("pause");
											return 1;
										}
										else //���� ��������� �������� ������������
										{
											for (int i = 0; i < strlen(instruction); i++) //��������� ������ ������
											{
												if (!(instruction[i] >= 48 && instruction[i] <= 57)) //���� ������ �� �������� ������
												{
													//������ ������ � ����� �� �������
													cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ������." << endl;
													system("pause");
													return 1;
												}
											}

											instruction = strtok(NULL, " "); //������� "to"

											if (instruction == NULL) //���� "to" �����������
											{
												//������ ������ � ����� �� �������
												cerr << "������ " << numStr << ": ����������� �������� \"to\"." << endl;
												system("pause");
												return 1;
											}
											else if (strcmp(instruction, "to") != 0) //���� ������� �� "to"
											{
												//������ ������ � ����� �� �������
												cerr << "������ " << numStr << ": �������� �������� \"to\" ������ \"" << instruction << "\"." << endl;
												system("pause");
												return 1;
											}
											else //���� "to" ������������
											{
												instruction = strtok(NULL, " "); //������� �������� ��������

												if (instruction == NULL) //���� �������� �������� �����������
												{
													//������ ������ � ����� �� �������
													cerr << "������ " << numStr << ": ����������� �������� �������� ��������� � ����� \"for\"." << endl;
													system("pause");
													return 1;
												}
												else //���� �������� �������� ������������
												{
													for (int i = 0; i < strlen(instruction); i++) //��������� ������ ������
													{
														if (!(instruction[i] >= 48 && instruction[i] <= 57)) //���� ������ �� �������� ������
														{
															//������ ������ � ����� �� �������
															cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ������." << endl;
															system("pause");
															return 1;
														}
													}

													instruction = strtok(NULL, " "); //������� �������

													if (instruction != NULL) //���� �� �������� ����� ������
													{
														if (strcmp(instruction, "step") != 0) //���� ������� �� �������� "step"
														{
															//������ ������ � ����� �� �������
															cerr << "������ " << numStr << ": �������� �������� \"step\" ������ \"" << instruction << "\"." << endl;
															system("pause");
															return 1;
														}
														else //���� ������� �������� "step"
														{
															instruction = strtok(NULL, " "); //���������� ��� �����

															if (instruction != NULL) //���� ��� ����� ������������
															{
																for (int i = 0; i < strlen(instruction); i++) //��������� ������ ������
																{
																	if (!(instruction[i] >= 48 && instruction[i] <= 57)) //������ �� �������� ������
																	{
																		//������ ������ � ����� �� �������
																		cerr << "������ " << numStr << ": \"" << instruction << "\" �� �������� ������." << endl;
																		system("pause");
																		return 1;
																	}
																}
															}
															else //���� ��� ����� �����������
															{
																//������ ������ � ����� �� �������
																cerr << "������ " << numStr << ": �� ������ ��� �����." << endl;
																system("pause");
																return 1;
															}
														}
													}
												}
											}
										}
									}
									else //���� ������� �� '='
									{
										//������ ������ � ����� �� �������
										cerr << "������ " << numStr << ": �������� �������� \'=\' ������ \"" << instruction << "\"." << endl;
										system("pause");
										return 1;
									}
								}
							}
							else //���� ������� �� ���������
							{
								//������ ������ � ����� �� �������
								cerr << "������ " << numStr << ": ��������� ���������� ������ \"" << instruction << "\", � �������� ���������." << endl;
								system("pause");
								return 1;
							}
						}
						else //���� ����� ������� ����� ������ �������
						{
							//������ ������ � ����� �� �������
							cerr << "������ " << numStr << ": ��������� ���������� ������ \"" << instruction << "\", � �������� ���������." << endl;
							system("pause");
							return 1;
						}
					}
				}
				else //���� ��������� ����������� �������
				{
					//������ ������ � ����� �� �������
					cerr << "������ " << numStr << ": ����������� �������� \"" << instruction << "\"." << endl;
					system("pause");
					return 1;
				}
			}
			else //���� ��������� ����������� �������
			{
				//������ ������ � ����� �� �������
				cerr << "������ " << numStr << ": ����������� �������� \"" << instruction << "\"." << endl;
				system("pause");
				return 1;
			}

			instruction = strtok(NULL, " "); //������� ��������� �������
		}
	}

	inputFile.seekg(0); //����������� ��������� ���������� � ������ �����
	return 0; //�������� ���������� �������
}

//���������, �������� �� ������ ���������� ����������
bool isLogical(char *str)
{
	return (
		   (strcmp(str, "==") == 0) ||
		   (strcmp(str, "!=") == 0) ||
		   (strcmp(str, ">=") == 0) ||
		   (strcmp(str, "<=") == 0) ||
		   (strcmp(str, ">") == 0)  ||
		   (strcmp(str, "<") == 0) 
		   );
}

//���������, �������� �� ������ ������ ����������
bool isVar(char ch)
{
	return ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122));
}

//���������, �������� �� ������ �����
bool isZero(char ch)
{
	return (ch == 48);
}