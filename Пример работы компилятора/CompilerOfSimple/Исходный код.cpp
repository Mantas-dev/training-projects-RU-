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

//КОНСТАНТЫ
const int BUFF_SIZE = 100; //Размер буфера
const int TABLE_SIZE = 100; //Размер таблицы символов

//МАССИВЫ
int instructionsSML[TABLE_SIZE] = {0}; //Массив, содержащий SML-инструкции для исполнения их Симплтроном
int flags[TABLE_SIZE]; //Массив, содержащий адреса неразрешенных ссылок при первом проходе компилятора
char buffer[BUFF_SIZE]; //Буфер
string funcNames[TABLE_SIZE]; //Массив, содержащий мена функций, встреченных в программе

//ОБЪЕКТЫ
StackInfix infixExpr; //Объект класса, производящий перевод выражения из инфиксной записи в постфиксную
StackPostfix postfixExpr; //Объект класса, производящий оценку постфиксного выражения. Создает SML-инструкции для Симплтрона
tableEntry symbolTable[TABLE_SIZE]; //Массив объектов класса, представляющий таблицу символов. Структура класса: хранимый символ (номер строки, ASCII - представление переменной, константа), тип символа, адрес в ячейке памяти

//ПЕРЕМЕННЫЕ
bool firstLexeme = true; //По определению языка, первая лексема каждой строки должна являться ее номером. Переменная определяет, была ли прочитана первая лексема в строке с помощью оператора strtok

int strNum = 0; //Текущий номер строки в коде программы
int var = 0; //Хранит имя переменнойб константу или номер строки, чтобы произвести поиск в таблице символов
int code = 0; //Хранит SML-инструкцию для дальнейшей записи в массив
int rowWithValue = 0; //Хранит номер строки из таблицы символов, возращенной функцией findSymbol

int currentRowTable = 0; //Текущая строка таблицы
int instructionAddress = 0; //Адрес инструкции
int varAddress = TABLE_SIZE; //Определяет адрес для переменной, который запишется в соответствующую позицию массива с SML-инструкциями
int curInstrSML = 0; //Хранит номер текущей SML-инструкции

int firstOperand = 0, secondOperand = 0; //Хранят операнды, используемые в условном операторе if ... goto

//УКАЗАТЕЛИ
char* instruction; //Хранит лексему, возвращенную оператором strtok
char anchor[BUFF_SIZE]; //Сохраняет дубликат строки для использования в функции notEqual

//ФАЙЛЫ
ifstream inputFile("../Debug/code.smp", ios_base::in); //Файл, хранящий программу до компиляции
ofstream outputFile("../Debug/temp_code.sml", ios_base::out); //Файл, хранящий SML-инструкции после компиляции

//ФУНКЦИИ
void firstProcess(); //Осуществляет первый проход компилятора
void secondProcess(); //Осуществляет второй проход компилятора
void equality(); //Создает инструкции, определяющие равенство двух операндов
void notEqual(); //Создает инструкции, определяющие неравенство двух операндов
void greaterOrEqual(); //Создает инструкции, определяющие больше ли один операнд чем другой или они равны
void lowerOrEqual(); //Создает инструкции, определяющие меньше ли один операнд чем другой или они равны
void greater(); //Создает инструкции, определяющие больше ли один операнд чем другой
void lower(); //Создает инструкции, определяющие меньше ли один операнд чем другой или они равны
void calculation(); //Создает SML-инструкции, соответствующие арифметическому выражению
void copyInArray(int &, char *, char *); //Копирует данные из одного массива символов в другой массив символов
void functionExecute(); //Создает инструкции, определяющие тело функции
void cycleFor(); //Создает инструкции, определяющие тело цикла for
int checkSyntax(); //Проверяет код программы на наличие синтаксических ошибок

bool isOperator(char); //Проверяет, является ли символ арифметическим оператором
bool isLogical(char *); //Проверяет, является ли символ логическим оператором
bool isVar(char); //Проверяет, является ли символ именем переменной
bool isZero(char); //Проверяет, является ли символ нулем

int findSymbol(int &, char); //Ищет символ в таблице символов

int main()
{
	setlocale(LC_ALL, "Russian");

	for (int i = 0; i < TABLE_SIZE; i++) flags[i] = -1; //Инициализация элементов массимва flags значением -1

	if (checkSyntax() == 0) //Если при проверке синтаксиса не выявлено ошибок - начать процесс компиляции
	{
		firstProcess(); //Вызвать функцию firstProcess

		secondProcess(); //Вызвать функцию secondProcess

		cout << "Компиляция программы выполнена без ошибок."<< endl; //Вывести сообщение об успешной компиляции

		system("pause");

	}

	return 0;
}

//Первый проход компилятора
void firstProcess()
{	
	while (!inputFile.eof()) //Пока не достигнут конец файла
	{
		inputFile.getline(buffer, BUFF_SIZE); //Считать строку из файла

		strcpy(anchor, buffer); //Сохранение дубликата строки

		instruction = strtok(buffer, " "); //Считать первую лексему

		firstLexeme = true; //Указать, что лексема является первой в строке

		while (instruction != NULL) //Пока вся строка не разделена на лексемы
		{
			if (firstLexeme) //Если принята первая лексема
			{
				strNum = atoi(instruction); //Сохранить номер строки

				if ((rowWithValue = findSymbol(strNum, 'L')) == -1) //Если текущий номер строки не найден в таблице символов
				{
					symbolTable[currentRowTable].setSymbol(strNum); //Поместить номер строки
				}
				firstLexeme = false; //Следующая прочитанная лексема из строки не является первой
			}

			if (strcmp(instruction,"rem") == 0) //Прочитаная лексема - "rem". Остальная часть строки игнорируется
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ
				break; //Выход из цикла по считыванию лексем
			}

			/***************************************************************************/

			else if (strcmp(instruction, "input") == 0) //Ввод данных. После input идет ТОЛЬКО переменная
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				instruction = strtok(NULL, " "); //Считать следующую лексему

				while (instruction != NULL) //Пока не считана вся строка
				{
					var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
					{
						currentRowTable++; //Перейти на следующую строку в таблице
						varAddress--; //Определить новый адрес в памяти для переменной

						//Сохранение имени переменной в таблице вместе с требуемой информацией
						symbolTable[currentRowTable].setSymbol(var); 
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						code = (10 * 100) + varAddress; //Составить SML-инструкцию ввода переменной

						instructionsSML[curInstrSML] = code; //Записать SML-инструкцию в массив

						curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
					}
					else //Если найдена в таблице
					{
						code = (10 * 100) + symbolTable[rowWithValue].getLocation(); //Составить SML-инструкцию ввода переменной

						instructionsSML[curInstrSML] = code; //Записать SML-инструкцию в массив

						curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
					}

					instruction = strtok(NULL, " "); //Считать следующую лексему
				}

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			else if (strcmp(instruction, "if") == 0) //Условный оператор
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				instruction = strtok(NULL, " "); //Считать первый операнд

				if (atoi(instruction) != 0 || isZero(instruction[0])) //Первый операнд является числом
				{
					var = atoi(instruction); //Сохранить число в переменную
					
					if ((rowWithValue = findSymbol(var, 'C')) == -1) //Если константа не найдена в таблице символов
					{
						currentRowTable++; //Перейти на следующую строку в таблице
						varAddress--; //Определить новый адрес в памяти для константы

						//Сохранение константы в таблице вместе с требуемой информацией
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('C');
						symbolTable[currentRowTable].setLocation(varAddress);

						instructionsSML[varAddress] = var; //Сохранить значение константы в массиве SML-инструкций

						firstOperand = varAddress; //Сохранить в переменной адрес первого операнда 
					}
					else firstOperand = symbolTable[rowWithValue].getLocation(); //Если константа найдена - сохранить в переменной адрес первого операнда 
				}
				else //Первый операнд является переменной
				{
					var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
					{
						currentRowTable++; //Перейти на следующую строку в таблице
						varAddress--; //Определить новый адрес в памяти для переменной

						//Сохранение имени переменной в таблице вместе с требуемой информацией
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						firstOperand = varAddress; //Сохранить в переменной адрес первого операнда
					}
					else firstOperand = symbolTable[rowWithValue].getLocation(); //Если переменная найдена - сохранить в переменной адрес первого операнда
				}

				instruction = strtok(NULL, " "); //Считать следующую лексему

				if (strcmp(instruction, "==") == 0) //Если лексема - "=="
				{
					equality(); //Вызвать функцию equality
				}
				else if (strcmp(instruction, "!=") == 0) //Если лексема - "!="
				{
					notEqual(); //Вызвать функцию notEqual
				}
				else if (strcmp(instruction, ">=") == 0) //Если лексема - ">="
				{
					greaterOrEqual(); //Вызвать функцию greaterOrEqual
				}
				else if (strcmp(instruction, "<=") == 0) //Если лексема - "<="
				{
					lowerOrEqual(); //Вызвать функцию lowerOrEqual
				}
				else if (strcmp(instruction, ">") == 0) //Если лексема - '>'
				{
					greater(); //Вызвать функцию greater
				}
				else if (strcmp(instruction, "<") == 0) //Если лексема - '<'
				{
					lower(); //Вызвать функцию lower
				}
				
				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			/***************************************************************************/

			else if (strcmp(instruction, "let") == 0) //Вычиление выражения
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				calculation(); //Вызвать функцию calculation

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			/***************************************************************************/

			else if (strcmp(instruction, "goto") == 0) //Переход по ссылке
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				instruction = strtok(NULL, " "); //Считать адрес перехода. Адресом перехода служит номер строки.

				var = atoi(instruction); //Сохранить номер строки в переменной

				if ((rowWithValue = findSymbol(var, 'L')) == -1) //Если номер строки не найден в таблице, то такая ссылка считается неразрешенной. Будет создана инструкция перехода без адреса. Во время второго прохода все
																//ссылки будут инициализированы
				{
					flags[curInstrSML] = var; //Сохранить в элементе, соответствующем текущей SML-инструкции, массива flags номер строки для перехода 

					code = (40 * 100); //Создать инструкцию перехода без ссылки

					instructionsSML[curInstrSML] = code; //Записать инструкцию

					curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
				}
				else //Если номер строки найден в таблице
				{
					code = (40 * 100) + symbolTable[rowWithValue].getLocation(); //Создать инструкцию перехода по ссылке

					instructionsSML[curInstrSML] = code; //Записать инструкцию

					curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
				}

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			/***************************************************************************/

			else if (strcmp(instruction, "print") == 0) //Вывод строки или значения переменной на экран
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				instruction = strtok(NULL, " "); //Считать лексему

				while (instruction != NULL) //Пока не прочитана вся строка
				{
					if (instruction[0] == '"') //Если первый символ лексемы - двойные кавычки
					{
						char printString[100] = ""; //Создать символьный массив для хранения всей строки

						strcat(printString, instruction); //Сохранить лексему в массиве
						instruction = strtok(NULL, ""); //Прочитать остальную строку целиком
						if (instruction != NULL)
						{
							strcat(printString, " "); //Дабвить пробел в массив
							strcat(printString, instruction); //Сохранить лексему в массиве
						}

						int length = strlen(printString); //Определить длину строки

						instructionsSML[curInstrSML] = (90 * 100) + (length - 2); //Создать SML-инструкцию где указана длина строки без двойных кавычек
						curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

						for (int i = 1; i < length; i++) //Создавать инструкции вывода для каждого символа
						{
							instructionsSML[curInstrSML] = (90 * 100) + (int)printString[i];
							curInstrSML++;
						}
					}
					else //Встречена переменная
					{
						var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

						if ((rowWithValue = findSymbol(var, 'V')) != -1) //Если переменная найдена в таблице символов
						{
							code = (11 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вывода значения переменной

							instructionsSML[curInstrSML] = code; //Записать инструкцию

							curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
						}
						else if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
						{
							currentRowTable++; //Перейти на следующую строку в таблице
							varAddress--; //Определить новый адрес в памяти для переменной

							//Сохранение имени переменной в таблице вместе с требуемой информацией
							symbolTable[currentRowTable].setSymbol(var);
							symbolTable[currentRowTable].setType('V');
							symbolTable[currentRowTable].setLocation(varAddress);

							code = (11 * 100) + varAddress; //Создать SML-инструкцию вывода значения переменной

							instructionsSML[curInstrSML] = code; //Записать инструкцию

							curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
						}
					}

					instruction = strtok(NULL, " "); //Прочитать следующую лексему
				}

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			/***************************************************************************/

			else if (strcmp(instruction, "end") == 0) //Завершение программы
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				instructionsSML[curInstrSML] = 4300; //Создать инструкцию завершения выполнения программы
				curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			else if (strcmp(instruction, "gosub") == 0) //Оператор перехода к функции
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				functionExecute(); //Вызов функции functionExecute

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			else if (strcmp(instruction, "sub") == 0) //Объявление функции
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				while (strcmp(instruction, "return") != 0) //Пока не встречен оператор "return"
				{
					instruction = strtok(NULL, " "); //Прочитать следующую лексему

					if (instruction == NULL) //Если строка прочитана
					{
						inputFile.getline(buffer, BUFF_SIZE); //Прочитать новую строку из файла
						instruction = strtok(buffer, " "); //Считать лексему
					}
				}
			}

			else if (strcmp(instruction, "for") == 0) //Цикл for
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				cycleFor(); //Вызов функции cycleFor

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			instruction = strtok(NULL, " "); //Считать следующую лексему
		}

		if (currentRowTable > TABLE_SIZE) //Если текущая строка таблицы выходит за установленные границы
		{
			cerr << "Произошло переполнение таблицы символов." << endl; //Выдать ошибку
			system("pause");
			system("exit"); //Выход из программы
		}
		else
		{
			currentRowTable++; //Перейти к следующей строке в таблице символов 
		}
	}
}

//Второй проход компилятора
void secondProcess()
{
	for (int i = 0; i < TABLE_SIZE; i++) //Пройти по всем элементам массива flags
	{
		if (flags[i] != -1) //Если встречается элемент отличный от -1
		{
			if ((rowWithValue = findSymbol(flags[i], 'L')) != -1) //Если номер строки, содержащийся в flags[i], найден
			{
				instructionsSML[i] += symbolTable[rowWithValue].getLocation(); //Добавить к пустой инструкции перехода адрес перехода
			}
		}
	}

	outputFile << instructionsSML[0]; //Записать в файл первую инструкцию

	for (int i = 1; i < TABLE_SIZE; i++) //Записать в файл остальные инструкции с предшествующим знаком перехода на новую строку
	{
		outputFile << "\n" << instructionsSML[i];
	}
}

//Поиск символа
int findSymbol(int &searchSymbol, char type)
{
	for (int currentRow = 0; currentRow < TABLE_SIZE + 1; currentRow++) //Пройти по всей таблице символов
	{
		if (symbolTable[currentRow].getSymbol() == (char)tolower(searchSymbol) && symbolTable[currentRow].getType() == type) //Если найден требуемый символ с заданным типом - вернуть позицию в таблице
			return currentRow;
	}
	return -1; //Если символ не найден - вернуть -1
}

//Создание SML-инструкций, определяющие равенство двух операндов
void equality()
{
	code = (20 * 100) + varAddress; //Создать SML-инструкцию загрузки в аккумулятор левого операнда

	instructionsSML[curInstrSML] = code; //Записать инструкцию

	curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

	instruction = strtok(NULL, " "); //Считать второй операнд

	if (atoi(instruction) != 0 || isZero(instruction[0])) //Если второй операнд - число
	{
		var = atoi(instruction); //Сохранить число в переменной

		if ((rowWithValue = findSymbol(var, 'C')) == -1) //Если число не найденно в таблице
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для константы

			//Сохранение константы в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вычитания второго операнда из первого

			instructionsSML[curInstrSML] = code; //Записать инструкцию
		
			instructionsSML[varAddress] = var; //Записать значение константы

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
		else //Если число найдено в таблице
		{
			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вычитания второго операнда из первого

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
	}
	else //Если второй операнд - переменная 
	{
		var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

		if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для переменной

			//Сохранение переменной в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('V');
			symbolTable[currentRowTable].setLocation(varAddress);

			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вычитания второго операнда из первого

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
		else //Если переменная найдена в таблице
		{
			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вычитания второго операнда из первого

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
	}

	instruction = strtok(NULL, " "); //Пропустить goto
	instruction = strtok(NULL, " "); //Определить строку перехода

	var = atoi(instruction); //Сохранить номер строки в переменной

	if ((rowWithValue = findSymbol(var, 'L')) == -1) //Если номер строки не найден в таблице, то такая ссылка считается неразрешенной. Будет создана инструкция перехода без адреса. Во время второго прохода все
													 //ссылки будут инициализированы
	{
		flags[curInstrSML] = var; //Сохранить в элементе, соответствующем текущей SML-инструкции, массива flags номер строки для перехода 

		code = (42 * 100); //Создать SML-инструкцию перехода по нулю без ссылки

		instructionsSML[curInstrSML] = code; //Записать инструкцию

		curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
	}
	else //Если номер строки найден в таблице
	{
		code = (42 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию перехода по нулю

		instructionsSML[curInstrSML] = code; //Записать инструкцию

		curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
	}
}

//Создание SML-инструкций, определяющие неравенство двух операндов
void notEqual()
{
	greater(); //Вызов функции greater

	//Процесс разбиения дубликата строки на лексемы до второго операнда в логическом выражении
	instruction = strtok(anchor, " "); 
	instruction = strtok(NULL, " ");
	instruction = strtok(NULL, " ");
	instruction = strtok(NULL, " ");

	lower(); //Вызов функции greater
}

//Создание SML-инструкций, определяющие больше ли один операнд чем другой или они равны
void greaterOrEqual()
{
	instruction = strtok(NULL, " "); //Определение второго операнда

	if (atoi(instruction) != 0 || isZero(instruction[0])) //Если второй операнд - число
	{
		secondOperand = atoi(instruction); //Сохранить число в переменную

		if ((rowWithValue = findSymbol(secondOperand, 'C')) == -1) //Если число не найлено в таблице символов
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для константы

			//Сохранение константы в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(secondOperand);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = secondOperand; //Записать значение константы

			code = (20 * 100) + varAddress; //Создать SML-инструкцию загрузки в аккумулятор правого операнда

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
		else //Если число найдено в таблице
		{
			code = (20 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию загрузки в аккумулятор правого операнда

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
	}
	else //Если второй операнд - переменная
	{
		secondOperand = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

		if ((rowWithValue = findSymbol(secondOperand, 'V')) == -1) //Если переменная не найдена в таблице символов
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для переменной

			//Сохранение переменной в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(secondOperand);
			symbolTable[currentRowTable].setType('V');
			symbolTable[currentRowTable].setLocation(varAddress);

			code = (20 * 100) + varAddress; //Создать SML-инструкцию загрузки в аккумулятор правого операнда

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
		else
		{
			code = (20 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию загрузки в аккумулятор правого операнда

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
	}

	code = (31 * 100) + firstOperand; //Создать SML-инструкцию вычитания первого операнда из второго

	instructionsSML[curInstrSML] = code; //Записать инструкцию

	curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

	instruction = strtok(NULL, " "); //Пропустить goto
	instruction = strtok(NULL, " "); //Определить строку перехода

	var = atoi(instruction); //Сохранить номер строки в переменную

	if ((rowWithValue = findSymbol(var, 'L')) == -1) //Если номер строки не найден в таблице, то такая ссылка считается неразрешенной. Будет создана инструкция перехода без адреса. Во время второго прохода все
													 //ссылки будут инициализированы 
	{
		flags[curInstrSML] = var; //Сохранить в элементе, соответствующем текущей SML-инструкции, массива flags номер строки для перехода

		code = (41 * 100); //Создать SML-инструкцию перехода по минусу без ссылки

		instructionsSML[curInstrSML] = code; //Записать инструкцию

		curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

		flags[curInstrSML] = var; //Сохранить в элементе, соответствующем текущей SML-инструкции, массива flags номер строки для перехода

		code = (42 * 100); //Создать SML-инструкцию перехода по нулю без ссылки

		instructionsSML[curInstrSML] = code; //Записать инструкцию

		curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
	}
	else
	{
		code = (41 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию перехода по минусу

		instructionsSML[curInstrSML] = code; //Записать инструкцию

		curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

		code = (42 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию перехода по нулю

		instructionsSML[curInstrSML] = code; //Записать инструкцию

		curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
	}
}

//Создание SML-инструкций, определяющие меньше ли один операнд чем другой или они равны
void lowerOrEqual()
{
	code = (20 * 100) + firstOperand; //Создать SML-инструкцию загрузки в аккумулятор левого операнда

	instructionsSML[curInstrSML] = code; //Записать инструкцию

	curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

	instruction = strtok(NULL, " "); //Определение второго операнда

	if (atoi(instruction) != 0 || isZero(instruction[0])) //Если второй операнд - число
	{
		var = atoi(instruction); //Сохранить число в переменную

		if ((rowWithValue = findSymbol(var, 'C')) == -1) //Если число не найдено в таблице
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для константы

			//Сохранение константы в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = var; //Записать значение константы

			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вычитания второго операнда из первого

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
		else //число найдено в таблице
		{
			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вычитания второго операнда из первого

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
	}
	else //Если второй операнд - переменная
	{
		var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

		if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для переменной

			//Сохранение переменной в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('V');
			symbolTable[currentRowTable].setLocation(varAddress);

			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вычитания второго операнда из первого

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
		else
		{
			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вычитания второго операнда из первого

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
	}

	instruction = strtok(NULL, " "); //Пропустить goto
	instruction = strtok(NULL, " "); //Определить строку перехода

	var = atoi(instruction); //Сохранить номер строки в переменную

	if ((rowWithValue = findSymbol(var, 'L')) == -1) //Если номер строки не найден в таблице, то такая ссылка считается неразрешенной. Будет создана инструкция перехода без адреса. Во время второго прохода все
													 //ссылки будут инициализированы 
	{
		flags[curInstrSML] = var; //Сохранить в элементе, соответствующем текущей SML-инструкции, массива flags номер строки для перехода

		code = (41 * 100); //Создать SML-инструкцию перехода по минусу без ссылки

		instructionsSML[curInstrSML] = code; //Записать инструкцию

		curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкции

		flags[curInstrSML] = var; //Сохранить в элементе, соответствующем текущей SML-инструкции, массива flags номер строки для перехода

		code = (42 * 100); //Создать SML-инструкцию перехода по нулю без ссылки

		instructionsSML[curInstrSML] = code; //Записать инструкцию

		curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
	}
	else
	{
		code = (41 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию перехода по минусу

		instructionsSML[curInstrSML] = code; //Записать инструкцию

		curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

		code = (42 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию перехода по нулю

		instructionsSML[curInstrSML] = code; //Записать инструкцию

		curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
	}
}

//Создание SML-инструкций, определяющие больше ли один операнд чем другой
void greater()
{
	instruction = strtok(NULL, " "); //Определение второго операнда

	if (atoi(instruction) != 0 || isZero(instruction[0])) //Если второй операнд - число
	{
		secondOperand = atoi(instruction); //Сохранить число в переменную

		if ((rowWithValue = findSymbol(secondOperand, 'C')) == -1) //Если число не найдено в таблице символов
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для константы

			//Сохранение константы в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(secondOperand);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = secondOperand; //Записать значение константы

			code = (20 * 100) + varAddress; //Создать SML-инструкцию загрузки в аккумулятор правого операнда

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
		else //число найдено в таблице
		{
			code = (20 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию загрузки в аккумулятор правого операнда

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
	}
	else //Если второй операнд - переменная
	{
		secondOperand = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

		if ((rowWithValue = findSymbol(secondOperand, 'V')) == -1) //Если переменная не найдена в таблице символов
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для переменной

			//Сохранение переменной в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(secondOperand);
			symbolTable[currentRowTable].setType('V');
			symbolTable[currentRowTable].setLocation(varAddress);

			code = (20 * 100) + varAddress; //Создать SML-инструкцию загрузки в аккумулятор правого операнда

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
		else
		{
			code = (20 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию загрузки в аккумулятор правого операнда

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
	}

	code = (31 * 100) + firstOperand; //Создать SML-инструкцию вычитания первого операнда из второго

	instructionsSML[curInstrSML] = code; //Записать инструкцию

	curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

	instruction = strtok(NULL, " "); //Пропустить goto
	instruction = strtok(NULL, " "); //Определить строку перехода

	var = atoi(instruction); //Сохранить номер строки в переменную

	if ((rowWithValue = findSymbol(var, 'L')) == -1) //Если номер строки не найден в таблице, то такая ссылка считается неразрешенной. Будет создана инструкция перехода без адреса. Во время второго прохода все
													 //ссылки будут инициализированы 
	{
		flags[curInstrSML] = var; //Сохранить в элементе, соответствующем текущей SML-инструкции, массива flags номер строки для перехода

		code = (41 * 100); //Создать SML-инструкцию перехода по минусу без ссылки

		instructionsSML[curInstrSML] = code; //Записать инструкцию

		curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
	}
	else
	{
		code = (41 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию перехода по минусу

		instructionsSML[curInstrSML] = code; //Записать инструкцию

		curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
	}
}

//Создание SML-инструкций, определяющие меньше ли один операнд чем другой
void lower()
{
	code = (20 * 100) + firstOperand; //Создать SML-инструкцию загрузки в аккумулятор левого операнда

	instructionsSML[curInstrSML] = code; //Записать инструкцию

	curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

	instruction = strtok(NULL, " "); //Определение второго операнда

	if (atoi(instruction) != 0 || isZero(instruction[0])) //Если второй операнд - число
	{
		var = atoi(instruction); //Сохранить число в переменную

		if ((rowWithValue = findSymbol(var, 'C')) == -1) //Если число не найдено в таблице символов
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для константы

			//Сохранение константы в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = var; //Записать значение константы

			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вычитания второго операнда из первого

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++;
		}
		else //число найдено в таблице
		{
			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вычитания второго операнда из первого

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
	}
	else //Если второй операнд - переменная
	{
		var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

		if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для переменной

			//Сохранение переменной в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('V');
			symbolTable[currentRowTable].setLocation(varAddress);

			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вычитания второго операнда из первого

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
		else
		{
			code = (31 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вычитания второго операнда из первого

			instructionsSML[curInstrSML] = code; //Записать инструкцию

			curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
		}
	}

	instruction = strtok(NULL, " "); //Пропустить goto
	instruction = strtok(NULL, " "); //Определить строку перехода

	var = atoi(instruction); //Сохранить номер строки в переменную

	if ((rowWithValue = findSymbol(var, 'L')) == -1) //Если номер строки не найден в таблице, то такая ссылка считается неразрешенной. Будет создана инструкция перехода без адреса. Во время второго прохода все
													 //ссылки будут инициализированы 
	{
		flags[curInstrSML] = var; //Сохранить в элементе, соответствующем текущей SML-инструкции, массива flags номер строки для перехода

		code = (41 * 100); //Создать SML-инструкцию перехода по минусу без ссылки

		instructionsSML[curInstrSML] = code; //Записать инструкцию

		curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
	}
	else
	{
		code = (41 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию перехода по минусу

		instructionsSML[curInstrSML] = code; //Записать инструкцию

		curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
	}
}

//Создание SML-инструкций, соответствующие арифметическому выражению
void calculation()
{
	int length = 0; //Длина строки

	bool readEqualSign = false; //Определяет, был ли прочитан знак '='

	char* tmpArray = new char[50]; //Временный символьный массив, для хранения арифметического выражения

	char varBeforeEqualSign = ' '; //Переменная, находящаяся до знака '='

	int currentItem = 0; // Параметр функции, который хранит текущую позицию массива

	instruction = strtok(NULL, " "); //Считать переменную, находящуюся перед знаком '='

	while (instruction != NULL) //Пока не прочитана вся строка
	{
		if (instruction[0] == '(')
		{
			int length = strlen(instruction) - 1;

			char* tmpInstr = new char[length]; 

			for (int i = 1; i < strlen(instruction); i++) //Скопировать содержимое после скобки во временный массив
				tmpInstr[i - 1] = instruction[i];

			if (atoi(tmpInstr) != 0 || isZero(tmpInstr[0])) //Если лексема - число
			{
				var = atoi(tmpInstr); //Сохранить число в переменную

				if ((rowWithValue = findSymbol(var, 'C')) == -1) //Если константа не найдена в таблице символов
				{
					currentRowTable++; //Перейти на следующую строку в таблице
					varAddress--; //Определить новый адрес в памяти для константы

					//Сохранение константы в таблице вместе с требуемой информацией
					symbolTable[currentRowTable].setSymbol(var);
					symbolTable[currentRowTable].setType('C');
					symbolTable[currentRowTable].setLocation(varAddress);

					instructionsSML[varAddress] = var; //Записать значение константы

					if (readEqualSign) //Если знак '=' был прочитан ранее
					{
						copyInArray(currentItem, tmpArray, tmpInstr); //Скопировать текущую лексему во временный массив
					}
				}
				else if (readEqualSign) //Если константа найдена и если знак '=' был прочитан ранее
				{
					copyInArray(currentItem, tmpArray, tmpInstr); //Скопировать текущую лексему во временный массив
				}
			}
			else //Если лексема - переменная или оператор
			{
				if (isOperator(tmpInstr[0])) //Если лексема - оператор
				{
					if (readEqualSign) //Если знак '=' был прочитан ранее
					{
						copyInArray(currentItem, tmpArray, tmpInstr); //Скопировать текущую лексему во временный массив
					}
				}
				else if (isVar(tmpInstr[0])) //Если лексема - переменная
				{
					var = tolower(tmpInstr[0]); //Сохранить ASCII-код строчной переменной

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
					{
						currentRowTable++; //Перейти на следующую строку в таблице
						varAddress--; //Определить новый адрес в памяти для переменной

						//Сохранение переменной в таблице вместе с требуемой информацией
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						if (readEqualSign) //Если знак '=' был прочитан ранее
						{
							copyInArray(currentItem, tmpArray, tmpInstr); //Скопировать текущую лексему во временный массив
						}

					}
					else if (readEqualSign) //Если переменная найдена и если знак '=' был прочитан ранее
					{
						copyInArray(currentItem, tmpArray, tmpInstr); //Скопировать текущую лексему во временный массив
					}
				}
			}
			delete[] tmpInstr;
		}
		else
		{
			if (atoi(instruction) != 0 || isZero(instruction[0])) //Если лексема - число
			{
				var = atoi(instruction); //Сохранить число в переменную

				if ((rowWithValue = findSymbol(var, 'C')) == -1) //Если константа не найдена в таблице символов
				{
					currentRowTable++; //Перейти на следующую строку в таблице
					varAddress--; //Определить новый адрес в памяти для константы

					//Сохранение константы в таблице вместе с требуемой информацией
					symbolTable[currentRowTable].setSymbol(var);
					symbolTable[currentRowTable].setType('C');
					symbolTable[currentRowTable].setLocation(varAddress);

					instructionsSML[varAddress] = var; //Записать значение константы

					if (readEqualSign) //Если знак '=' был прочитан ранее
					{
						copyInArray(currentItem, tmpArray, instruction); //Скопировать текущую лексему во временный массив
					}
				}
				else if (readEqualSign) //Если константа найдена и если знак '=' был прочитан ранее
				{
					copyInArray(currentItem, tmpArray, instruction); //Скопировать текущую лексему во временный массив
				}
			}
			else //Если лексема - переменная или оператор
			{
				if (isOperator(instruction[0])) //Если лексема - оператор
				{
					if (readEqualSign) //Если знак '=' был прочитан ранее
					{
						copyInArray(currentItem, tmpArray, instruction); //Скопировать текущую лексему во временный массив
					}
				}
				else if (isVar(instruction[0])) //Если лексема - переменная
				{
					var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
					{
						currentRowTable++; //Перейти на следующую строку в таблице
						varAddress--; //Определить новый адрес в памяти для переменной

						//Сохранение переменной в таблице вместе с требуемой информацией
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						if (readEqualSign) //Если знак '=' был прочитан ранее
						{
							copyInArray(currentItem, tmpArray, instruction); //Скопировать текущую лексему во временный массив
						}

					}
					else if (readEqualSign) //Если переменная найдена и если знак '=' был прочитан ранее
					{
						copyInArray(currentItem, tmpArray, instruction); //Скопировать текущую лексему во временный массив
					}
				}
			}
		}
		if (strcmp(instruction, "=") == 0 && !readEqualSign) readEqualSign = true; //Если лексема - знак '=' и она не была прочитана ранее - отметить как прочитанную
		else if (strcmp(instruction, "=") != 0 && !readEqualSign) varBeforeEqualSign = var; //Если лексема - не знак '=' и знак '=' не был прочитан ранее - сохранить лексему как переменную, находящуюся перед знаком '='

		instruction = strtok(NULL, " "); //Считать следующую лексему
	}

	length = currentItem - 1; //Определить длину выражения

	char *infix = new char[length + 2]; //Массив, в котором хранится все выражение в инфиксной записи
	string *postfix = new string[length + 1]; //Массив, в котором хранится выражение в постфиксной записи

	for (int i = 0; i < length; i++) //Скопировать символы из tmpArray в infix
	{
		infix[i] = tmpArray[i];
	}

	//Добавить в конец символ '0' и завершающий символ '\0'
	infix[length] = '0';
	infix[length + 1] = '\0';

	infixExpr.convertToPostfix(infix, length + 2, postfix, length + 1); //Осуществить преобразование выражения в инфиксной записи в выражение в постфиксной записи

	var = (int)varBeforeEqualSign; // Сохранить переменную до знака '='

	postfixExpr.evaluatePostfixExpression(postfix, length + 1, symbolTable, TABLE_SIZE, instructionsSML, curInstrSML, varAddress); //Создать SML-инструкции из постфиксного выражения для выполнения их Симплтроном

	instructionsSML[curInstrSML] = (21 * 100) + symbolTable[findSymbol(var,'V')].getLocation(); //Создать и записать SML-инструкцию по выгрузке значения из аккумулятора в память
	curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

	delete[] tmpArray; delete[] infix, delete[] postfix; //Высвободить память от массивов, созданных путем выделение динамической памяти
}

//Копирование строки в символьный массив
void copyInArray(int &item, char* arr, char* str)
{
	for (int i = 0; i < strlen(str); i++) //Поэлементное копирование
	{
		arr[item] = str[i];
		item++; //Переход на следующий элемент массива
		arr[item] = ' '; //Добавление пробельного символа
		item++; //Переход на следующий элемент массива
	}
}

//Проверка соответствия символа одному из арифметических операторов
bool isOperator(char ch)
{
	return (ch == '=' || ch == '*' || ch == '/' || ch == '+' || ch == '-' || ch == '%' || ch == '^');
}

//Создание SML-инструкций, соответствующие операторам в теле функции
void functionExecute()
{
	bool wasRead = false; //Определяет, прочитано ли тело функции

	int length = strlen(instruction);

	//Считать название функции
	instruction = strtok(NULL, " ");

	char* func_name = new char[length]; //Создать массив для хранения имени функции

	strcpy(func_name, instruction); //Скопировать имя функции в массив

	std::ios::pos_type pos = inputFile.tellg(); //Сохранить текущую позицию чтения из файла

	inputFile.seekg(0); //Перейти в начало файла

	inputFile.getline(buffer, BUFF_SIZE); //Считать строку из файла

	instruction = strtok(buffer, " "); //Считать первую лексему

	while (strcmp(instruction, func_name) != 0) //Пока не найдено требуемое имя функции
	{
		instruction = strtok(NULL, " "); //Считать следующую лексему

		if (instruction == NULL) //Если вся строка прочитана
		{
			inputFile.getline(buffer, BUFF_SIZE); //Считать строку из файла

			instruction = strtok(buffer, " "); //Считать первую лексему
		}
	}

	//Считывание тела функции и создание SML-инструкций
	while (!wasRead)
	{
		inputFile.getline(buffer, BUFF_SIZE); //Считать строку из файла

		strcpy(anchor, buffer); //Сохранить дубликат строки

		instruction = strtok(buffer, " "); //Считать первую лексему

		firstLexeme = true; //Была прочитана первая лексема

		while (instruction != NULL) //Пока не прочитана вся строка
		{
			if (firstLexeme) //Если лексема - первая
			{
				strNum = atoi(instruction); //Сохранить номер строки

				if ((rowWithValue = findSymbol(strNum, 'L')) == -1) //Если номер строки не найден в таблице символов
				{
					symbolTable[currentRowTable].setSymbol(strNum);//Поместить номер строки
				}
				firstLexeme = false; //Следующая лексема не является первой
			}

			if (strcmp(instruction, "rem") == 0) //Прочитаная лексема - "rem". Остальная часть строки игнорируется
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ
				break; //Выход из цикла по считыванию лексем
			}

			else if (strcmp(instruction, "input") == 0) //Ввод данных. После input идет ТОЛЬКО переменная
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				instruction = strtok(NULL, " "); //Считать следующую лексему

				while (instruction != NULL) //Пока не считана вся строка
				{
					var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
					{
						currentRowTable++; //Перейти на следующую строку в таблице
						varAddress--; //Определить новый адрес в памяти для переменной

						//Сохранение имени переменной в таблице вместе с требуемой информацией
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						code = (10 * 100) + varAddress; //Составить SML-инструкцию ввода переменной

						instructionsSML[curInstrSML] = code; //Записать SML-инструкцию в массив

						curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
					}
					else //Если найдена в таблице
					{
						code = (10 * 100) + symbolTable[rowWithValue].getLocation(); //Составить SML-инструкцию ввода переменной

						instructionsSML[curInstrSML] = code; //Записать SML-инструкцию в массив

						curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
					}

					instruction = strtok(NULL, " "); //Считать следующую лексему
				}

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			else if (strcmp(instruction, "if") == 0) //Условный оператор
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				instruction = strtok(NULL, " "); //Считать первый операнд

				if (atoi(instruction) != 0 || isZero(instruction[0])) //Первый операнд является числом
				{
					var = atoi(instruction); //Сохранить число в переменную

					if ((rowWithValue = findSymbol(var, 'C')) == -1) //Если константа не найдена в таблице символов
					{
						currentRowTable++; //Перейти на следующую строку в таблице
						varAddress--; //Определить новый адрес в памяти для константы

						//Сохранение константы в таблице вместе с требуемой информацией
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('C');
						symbolTable[currentRowTable].setLocation(varAddress);

						instructionsSML[varAddress] = var; //Сохранить значение константы в массиве SML-инструкций

						firstOperand = varAddress; //Сохранить в переменной адрес первого операнда 
					}
					else firstOperand = symbolTable[rowWithValue].getLocation(); //Если константа найдена - сохранить в переменной адрес первого операнда 
				}
				else //Первый операнд является переменной
				{
					var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
					{
						currentRowTable++; //Перейти на следующую строку в таблице
						varAddress--; //Определить новый адрес в памяти для переменной

						//Сохранение имени переменной в таблице вместе с требуемой информацией
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						firstOperand = varAddress; //Сохранить в переменной адрес первого операнда
					}
					else firstOperand = symbolTable[rowWithValue].getLocation(); //Если переменная найдена - сохранить в переменной адрес первого операнда
				}

				instruction = strtok(NULL, " "); //Считать следующую лексему

				if (strcmp(instruction, "==") == 0) //Если лексема - "=="
				{
					equality(); //Вызвать функцию equality
				}
				else if (strcmp(instruction, "!=") == 0) //Если лексема - "!="
				{
					notEqual(); //Вызвать функцию notEqual
				}
				else if (strcmp(instruction, ">=") == 0) //Если лексема - ">="
				{
					greaterOrEqual(); //Вызвать функцию greaterOrEqual
				}
				else if (strcmp(instruction, "<=") == 0) //Если лексема - "<="
				{
					lowerOrEqual(); //Вызвать функцию lowerOrEqual
				}
				else if (strcmp(instruction, ">") == 0) //Если лексема - '>'
				{
					greater(); //Вызвать функцию greater
				}
				else if (strcmp(instruction, "<") == 0) //Если лексема - '<'
				{
					lower(); //Вызвать функцию lower
				}

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			/***************************************************************************/

			else if (strcmp(instruction, "let") == 0) //Вычиление выражения
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				calculation(); //Вызвать функцию calculation

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			/***************************************************************************/

			else if (strcmp(instruction, "goto") == 0) //Переход по ссылке
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				instruction = strtok(NULL, " "); //Считать адрес перехода. Адресом перехода служит номер строки.

				var = atoi(instruction); //Сохранить номер строки в переменной

				if ((rowWithValue = findSymbol(var, 'L')) == -1) //Если номер строки не найден в таблице, то такая ссылка считается неразрешенной. Будет создана инструкция перехода без адреса. Во время второго прохода все
					//ссылки будут инициализированы
				{
					flags[curInstrSML] = var; //Сохранить в элементе, соответствующем текущей SML-инструкции, массива flags номер строки для перехода 

					code = (42 * 100); //Создать инструкцию перехода без ссылки

					instructionsSML[curInstrSML] = code; //Записать инструкцию

					curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
				}
				else //Если номер строки найден в таблице
				{
					code = (40 * 100) + symbolTable[rowWithValue].getLocation(); //Создать инструкцию перехода по ссылке

					instructionsSML[curInstrSML] = code; //Записать инструкцию

					curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
				}

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			/***************************************************************************/

			else if (strcmp(instruction, "print") == 0) //Вывод строки или значения переменной на экран
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				instruction = strtok(NULL, " "); //Считать лексему

				while (instruction != NULL) //Пока не прочитана вся строка
				{
					if (instruction[0] == '"') //Если первый символ лексемы - двойные кавычки
					{
						char printString[100] = ""; //Создать символьный массив для хранения всей строки

						strcat(printString, instruction); //Сохранить лексему в массиве
						instruction = strtok(NULL, ""); //Прочитать остальную строку целиком
						if (instruction != NULL)
						{
							strcat(printString, " "); //Дабвить пробел в массив
							strcat(printString, instruction); //Сохранить лексему в массиве
						}

						int length = strlen(printString); //Определить длину строки

						instructionsSML[curInstrSML] = (90 * 100) + (length - 2); //Создать SML-инструкцию где указана длина строки без двойных кавычек
						curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

						for (int i = 1; i < length; i++) //Создавать инструкции вывода для каждого символа
						{
							instructionsSML[curInstrSML] = (90 * 100) + (int)printString[i];
							curInstrSML++;
						}
					}
					else //Встречена переменная
					{
						var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

						if ((rowWithValue = findSymbol(var, 'V')) != -1) //Если переменная найдена в таблице символов
						{
							code = (11 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вывода значения переменной

							instructionsSML[curInstrSML] = code; //Записать инструкцию

							curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
						}
						else if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
						{
							currentRowTable++; //Перейти на следующую строку в таблице
							varAddress--; //Определить новый адрес в памяти для переменной

							//Сохранение имени переменной в таблице вместе с требуемой информацией
							symbolTable[currentRowTable].setSymbol(var);
							symbolTable[currentRowTable].setType('V');
							symbolTable[currentRowTable].setLocation(varAddress);

							code = (11 * 100) + varAddress; //Создать SML-инструкцию вывода значения переменной

							instructionsSML[curInstrSML] = code; //Записать инструкцию

							curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
						}
					}

					instruction = strtok(NULL, " "); //Прочитать следующую лексему
				}

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			/***************************************************************************/

			else if (strcmp(instruction, "for") == 0) //Цикл for
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				cycleFor(); //Вызов функции cycleFor

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			/***************************************************************************/

			else if (strcmp(instruction, "gosub") == 0) //Оператор перехода к функции
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				functionExecute(); //Вызов функции functionExecute

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			else if (strcmp(instruction, "return") == 0) wasRead = true; //Если встречен оператор "return" - отметить, что тело функции прочитано

			instruction = strtok(NULL, " "); //Прочитать следующую лексему
		}

		if (currentRowTable > TABLE_SIZE) //Если текущая строка таблицы выходит за установленные границы
		{
			cerr << "Произошло переполнение таблицы символов." << endl; //Выдать ошибку
			system("pause");
			system("exit"); //Выход из программы
		}
		else
		{
			currentRowTable++; //Перейти к следующей строке в таблице символов 
		}
	}

	inputFile.seekg(pos); //После считывания тела функции - перейти к строке файла, следующей за вызовом функции оператором "gosub"

	delete[] func_name; //Высвободить память от динамического символьного массива
}

//Создание SML-инструкций, соответствующие операторам в теле цикла for
void cycleFor()
{
	bool wasRead = false; // //Определяет, прочитано ли тело цикла

	//parameter - адрес переменной-параметра; cycleStart - начало цикла; rightSide - конец цикла; step - шаг цикла
	int parameter = 0, cycleStart = -1, rightSide = 0, step = 0;

	instruction = strtok(NULL, " "); //Считать лексему

	var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

	if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
	{
		currentRowTable++; //Перейти на следующую строку в таблице
		varAddress--; //Определить новый адрес в памяти для переменной

		//Сохранение имени переменной в таблице вместе с требуемой информацией
		symbolTable[currentRowTable].setSymbol(var);
		symbolTable[currentRowTable].setType('V');
		symbolTable[currentRowTable].setLocation(varAddress);

		parameter = varAddress; //Сохранить адрес переменной 
	}
	else //Если переменная найдена в таблице символов
	{
		parameter = symbolTable[rowWithValue].getLocation(); //Сохранить адрес переменной 
	}

	instruction = strtok(NULL, " "); //Пропустить '='
	instruction = strtok(NULL, " "); //Прочитать начальное значение

	if (atoi(instruction) != 0 || isZero(instruction[0])) //Если лексема - число
	{
		var = atoi(instruction); //Сохранить число в переменную

		if ((rowWithValue = findSymbol(var, 'C')) == -1) //Если константа не найдена в таблице символов
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для константы

			//Сохранение имени константы в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = var; //Записать константу в SML-массив
		}
	}
	else //Если лексема - переменная
	{
		var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

		if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для переменной

			//Сохранение имени переменной в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('V');
			symbolTable[currentRowTable].setLocation(varAddress);
		}
	}

	instructionsSML[parameter] = atoi(instruction); //Сохранить значение начала цикла в SML-массиве

	instruction = strtok(NULL, " "); //Пропустить "to"
	instruction = strtok(NULL, " "); //Считать конечное значение

	if (atoi(instruction) != 0 || isZero(instruction[0])) //Если лексема - число
	{
		var = atoi(instruction); //Сохранить число в переменную

		if ((rowWithValue = findSymbol(var, 'C')) == -1) //Если константа не найдена в таблице символов
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для константы

			//Сохранение имени константы в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = var; //Записать константу в SML-массив

			rightSide = varAddress; //Сохранить адрес значения конца цикла в SML-массиве
		}
		else //Если константа найдена в таблице символов
		{
			rightSide = symbolTable[rowWithValue].getLocation(); //Сохранить адрес значения конца цикла в SML-массиве
		}
	}
	else //Если лексема - переменная
	{
		var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

		if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для переменной

			//Сохранение имени переменной в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('V');
			symbolTable[currentRowTable].setLocation(varAddress);

			rightSide = varAddress; //Сохранить адрес значения конца цикла в SML-массив
		}
		else  //Если переменная найдена в таблице символов
		{
			rightSide = symbolTable[rowWithValue].getLocation(); //Сохранить адрес значения конца цикла в SML-массив
		}

	}

	instruction = strtok(NULL, " "); //Определить, имеется ли оператор "step"

	if (instruction == NULL) //Если оператор "step" отсутствует
	{
		var = 1; //Сохранить шаг цикла в переменную

		if ((rowWithValue = findSymbol(var, 'C')) == -1) //Если число не найдено в таблице символов
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для константы

			//Сохранение имени константы в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = var; //Записать константу в SML-массив
									
			step = varAddress; //Сохранить адрес значения шага цикла в SML-массиве
		}
		else //Если число найдено в таблице символов
		{
			step = symbolTable[rowWithValue].getLocation(); //Сохранить адрес значения шага цикла в SML-массиве
		}
	}
	else //Если оператор "step" присутствует
	{
		instruction = strtok(NULL, " "); //Определить размер шага цикла

		var = atoi(instruction); //Сохранить шаг цикла в переменную

		if ((rowWithValue = findSymbol(var, 'C')) == -1) //Если число не найдено в таблице символов
		{
			currentRowTable++; //Перейти на следующую строку в таблице
			varAddress--; //Определить новый адрес в памяти для константы

			//Сохранение имени константы в таблице вместе с требуемой информацией
			symbolTable[currentRowTable].setSymbol(var);
			symbolTable[currentRowTable].setType('C');
			symbolTable[currentRowTable].setLocation(varAddress);

			instructionsSML[varAddress] = var; //Записать константу в SML-массив

			step = varAddress; //Сохранить адрес значения шага цикла в SML-массиве
		}
		else
		{
			step = symbolTable[rowWithValue].getLocation(); //Сохранить адрес значения шага цикла в SML-массиве
		}
	}

	//Считывание операторов цикла до оператора "next"
	while (!wasRead)
	{
		inputFile.getline(buffer, BUFF_SIZE); //Счиать строку из файла

		strcpy(anchor, buffer); //Сохранить дубликат строки

		instruction = strtok(buffer, " "); //Считать первую лексему

		firstLexeme = true; //Была прочитана первая лексема

		while (instruction != NULL) //Пока не прочитана вся строка
		{
			if (firstLexeme) //Если лексема - первая
			{
				strNum = atoi(instruction); //Сохранить номер строки

				if ((rowWithValue = findSymbol(strNum, 'L')) == -1) //Если номер строки не найден в таблице символов
				{
					symbolTable[currentRowTable].setSymbol(strNum); //Поместить номер строки
				}

				if (cycleStart == -1) cycleStart = instructionAddress; //Задать начало цикла

				firstLexeme = false; //Следующая лексема не является первой
			}

			if (strcmp(instruction, "rem") == 0) //Прочитаная лексема - "rem". Остальная часть строки игнорируется
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ
				break; //Выход из цикла по считыванию лексем
			}

			else if (strcmp(instruction, "input") == 0) //Ввод данных. После input идет ТОЛЬКО переменная
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				instruction = strtok(NULL, " "); //Считать следующую лексему

				while (instruction != NULL) //Пока не считана вся строка
				{
					var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
					{
						currentRowTable++; //Перейти на следующую строку в таблице
						varAddress--; //Определить новый адрес в памяти для переменной

						//Сохранение имени переменной в таблице вместе с требуемой информацией
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						code = (10 * 100) + varAddress; //Составить SML-инструкцию ввода переменной

						instructionsSML[curInstrSML] = code; //Записать SML-инструкцию в массив

						curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
					}
					else //Если переменная найдена в таблице символов
					{
						code = (10 * 100) + symbolTable[rowWithValue].getLocation(); //Составить SML-инструкцию ввода переменной

						instructionsSML[curInstrSML] = code; //Записать SML-инструкцию в массив

						curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
					}

					instruction = strtok(NULL, " "); //Считать следующую лексему
				}

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			else if (strcmp(instruction, "if") == 0) //Условный оператор
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				instruction = strtok(NULL, " "); //Считать первый операнд

				if (atoi(instruction) != 0 || isZero(instruction[0])) //Первый операнд является числом
				{
					var = atoi(instruction); //Сохранить число в переменную

					if ((rowWithValue = findSymbol(var, 'C')) == -1) //Если константа не найдена в таблице символов
					{
						currentRowTable++; //Перейти на следующую строку в таблице
						varAddress--; //Определить новый адрес в памяти для константы

						//Сохранение константы в таблице вместе с требуемой информацией
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('C');
						symbolTable[currentRowTable].setLocation(varAddress);

						instructionsSML[varAddress] = var; //Сохранить значение константы в массиве SML-инструкций

						firstOperand = varAddress; //Сохранить в переменной адрес первого операнда 
					}
					else firstOperand = symbolTable[rowWithValue].getLocation(); //Если константа найдена - сохранить в переменной адрес первого операнда
				}
				else //Первый операнд является переменной
				{
					var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

					if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
					{
						currentRowTable++; //Перейти на следующую строку в таблице
						varAddress--; //Определить новый адрес в памяти для переменной

						//Сохранение имени переменной в таблице вместе с требуемой информацией
						symbolTable[currentRowTable].setSymbol(var);
						symbolTable[currentRowTable].setType('V');
						symbolTable[currentRowTable].setLocation(varAddress);

						firstOperand = varAddress; //Сохранить в переменной адрес первого операнда
					}
					else firstOperand = symbolTable[rowWithValue].getLocation(); //Если переменная найдена - сохранить в переменной адрес первого операнда
				}

				instruction = strtok(NULL, " "); //Считать следующую лексему

				if (strcmp(instruction, "==") == 0) //Если лексема - "=="
				{
					equality(); //Вызвать функцию equality
				}
				else if (strcmp(instruction, "!=") == 0) //Если лексема - "!="
				{
					notEqual(); //Вызвать функцию notEqual
				}
				else if (strcmp(instruction, ">=") == 0) //Если лексема - ">="
				{
					greaterOrEqual(); //Вызвать функцию greaterOrEqual
				}
				else if (strcmp(instruction, "<=") == 0) //Если лексема - "<="
				{
					lowerOrEqual(); //Вызвать функцию lowerOrEqual
				}
				else if (strcmp(instruction, ">") == 0) //Если лексема - '>'
				{
					greater(); //Вызвать функцию greater
				}
				else if (strcmp(instruction, "<") == 0) //Если лексема - '<'
				{
					lower(); //Вызвать функцию lower
				}

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			/***************************************************************************/

			else if (strcmp(instruction, "let") == 0) //Вычиление выражения
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				calculation(); //Вызвать функцию calculation

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			/***************************************************************************/

			else if (strcmp(instruction, "goto") == 0) //Переход по ссылке
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				instruction = strtok(NULL, " "); //Считать адрес перехода. Адресом перехода служит номер строки.

				var = atoi(instruction); //Сохранить номер строки в переменной

				if ((rowWithValue = findSymbol(var, 'L')) == -1) //Если номер строки не найден в таблице, то такая ссылка считается неразрешенной. Будет создана инструкция перехода без адреса. Во время второго прохода все
					//ссылки будут инициализированы
				{
					flags[curInstrSML] = var; //Сохранить в элементе, соответствующем текущей SML-инструкции, массива flags номер строки для перехода 

					code = (42 * 100); //Создать инструкцию перехода без ссылки

					instructionsSML[curInstrSML] = code; //Записать инструкцию

					curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
				}
				else //Если номер строки найден в таблице
				{
					code = (40 * 100) + symbolTable[rowWithValue].getLocation(); //Создать инструкцию перехода по ссылке

					instructionsSML[curInstrSML] = code; //Записать инструкцию

					curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
				}

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			/***************************************************************************/

			else if (strcmp(instruction, "print") == 0) //Вывод строки или значения переменной на экран
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				instruction = strtok(NULL, " "); //Считать лексему

				while (instruction != NULL) //Пока не прочитана вся строка
				{
					if (instruction[0] == '"') //Если первый символ лексемы - двойные кавычки
					{
						char printString[100] = ""; //Создать символьный массив для хранения всей строки

						strcat(printString, instruction); //Сохранить лексему в массиве
						instruction = strtok(NULL, ""); //Прочитать остальную строку целиком
						if (instruction != NULL)
						{
							strcat(printString, " "); //Дабвить пробел в массив
							strcat(printString, instruction); //Сохранить лексему в массиве
						}

						int length = strlen(printString); //Определить длину строки

						instructionsSML[curInstrSML] = (90 * 100) + (length - 2); //Создать SML-инструкцию где указана длина строки без двойных кавычек
						curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

						for (int i = 1; i < length; i++) //Создавать инструкции вывода для каждого символа
						{
							instructionsSML[curInstrSML] = (90 * 100) + (int)printString[i];
							curInstrSML++;
						}
					}
					else //Встречена переменная
					{
						var = tolower(instruction[0]); //Сохранить ASCII-код строчной переменной

						if ((rowWithValue = findSymbol(var, 'V')) != -1) //Если переменная найдена в таблице символов
						{
							code = (11 * 100) + symbolTable[rowWithValue].getLocation(); //Создать SML-инструкцию вывода значения переменной

							instructionsSML[curInstrSML] = code; //Записать инструкцию

							curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
						}
						else if ((rowWithValue = findSymbol(var, 'V')) == -1) //Если переменная не найдена в таблице символов
						{
							currentRowTable++; //Перейти на следующую строку в таблице
							varAddress--; //Определить новый адрес в памяти для переменной

							//Сохранение имени переменной в таблице вместе с требуемой информацией
							symbolTable[currentRowTable].setSymbol(var);
							symbolTable[currentRowTable].setType('V');
							symbolTable[currentRowTable].setLocation(varAddress);

							code = (11 * 100) + varAddress; //Создать SML-инструкцию вывода значения переменной

							instructionsSML[curInstrSML] = code; //Записать инструкцию

							curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
						}
					}

					instruction = strtok(NULL, " "); //Прочитать следующую лексему
				}

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			/***************************************************************************/

			else if (strcmp(instruction, "gosub") == 0) //Оператор перехода к функции
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				functionExecute(); //Вызов функции functionExecute

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			/***************************************************************************/

			else if (strcmp(instruction, "for") == 0) //Цикл for
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				cycleFor(); //Вызов функции cycleFor

				instructionAddress = curInstrSML; //Сохранить в переменную новый адрес следующей SML-инструкции
			}

			else if (strcmp(instruction, "next") == 0)
			{
				symbolTable[currentRowTable].setType('L'); //Поместить в текущей строке таблицы символов тип символа (строка)
				symbolTable[currentRowTable].setLocation(instructionAddress); //Поместить в текущей строке таблицы символов номер ячейки, на которую ссылается символ

				wasRead = true; //Если встречен оператор "return" - отметить, что тело цикла прочитано
			}
			instruction = strtok(NULL, " "); //Прочитать следующую лексему
		}

		if (currentRowTable > TABLE_SIZE) //Если текущая строка таблицы выходит за установленные границы
		{
			cerr << "Произошло переполнение таблицы символов." << endl; //Выдать ошибку
			system("pause");
			system("exit"); //Выход из программы
		}
		else
		{
			currentRowTable++; //Перейти к следующей строке в таблице символов 
		}
	}

	instructionsSML[curInstrSML] = (20 * 100) + parameter; //Записать SML-инструкцию загрузки в аккумулятор значения параметра
	curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

	instructionsSML[curInstrSML] = (30 * 100) + step; //Записать SML-инструкцию сложения параметра и шага цикла
	curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

	instructionsSML[curInstrSML] = (21 * 100) + parameter; //Записать SML-инструкцию выгрузки из аккумулятора значения параметра
	curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций



	instructionsSML[curInstrSML] = (20 * 100) + parameter; //Записать SML-инструкцию загрузки в аккумулятор значения параметра
	curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

	instructionsSML[curInstrSML] = (31 * 100) + rightSide; //Записать SML-инструкцию вычитания значения конца границы цикла из аккумулятора 
	curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

	instructionsSML[curInstrSML] = (41 * 100) + cycleStart; //Записать SML-инструкцию перехода по минусу
	curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций

	instructionsSML[curInstrSML] = (42 * 100) + cycleStart; //Записать SML-инструкцию перехода по нулю
	curInstrSML++; //Перейти на пустой элемент в массиве SML-инструкций
}

int checkSyntax()
{
	int numStr = 0; //Определяет номер строки, чтобы вывести его в сообщении об ошибке
	bool endWasFound = false; //Проверка на количество операторов end в программе (end может встречаться только один раз).
	int countUnfinishedFor = 0; //Определяет количество незавершенных циклов FOR
	int strWhenSubBegin = 0; //Сохраняет позицию начала объявления функции и обнуляется, когда встречает return

	while (!inputFile.eof()) //Пока не достигнут конец файла
	{
		numStr = 0; //Задать начальное значения для переменной

		inputFile.getline(buffer, BUFF_SIZE); //Считать строку из файла

		instruction = strtok(buffer, " "); //Считать первую лексему

		firstLexeme = true; //Была прочитана первая лексема

		while (instruction != NULL) //Пока не прочитана вся строка
		{
			if (firstLexeme) //Если прочитанная лексема - первая
			{
				if (atoi(instruction) == 0) //Первая лексема не номер строки
				{
					//Выдать ошибку и выйти из функции
					cerr << "Ожидался номер строки вместо \"" << instruction << "\"." << endl;
					system("pause");
					return 1;
				}
				else //Первая лексема - номер строки
				{
					firstLexeme = false; //Следующая прочитанная лексема не является первой
					numStr = atoi(instruction); //Сохранить номер строки
					instruction = strtok(NULL, " "); //Прочитать следующую лексему
					continue; //Начать следующий шаг цикла
				}
			}

			if (strlen(instruction) == 2) //Если длина лексемы - два символа
			{
				if (strcmp(instruction, "if") == 0) //Если лексема  - оператор "if"
				{
					instruction = strtok(NULL, " "); //Определить первый сравниваемый операнд 

					if (instruction == NULL) //Если операнд отсутствует
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Требуется выражение после оператора \"if\"." << endl;
						system("pause");
						return 1;
					}
					else //Если операнд присутствует
					{
						if (strlen(instruction) == 1) //Если длина лексемы - один символ
						{
							if (atoi(instruction) != 0) //Первый операнд - число больше 0
							{
								instruction = strtok(NULL, " "); //Определить логический оператор

								if (instruction == NULL) //Если логический оператор отсутствует
								{
									//Выдать ошибку и выйти из функции
									cerr << "Строка " << numStr << ": Отсутствует логический оператор." << endl;
									system("pause");
									return 1;
								}
								else //Если логический оператор присутствует
								{
									if (isLogical(instruction)) //Если это логический оператор
									{
										instruction = strtok(NULL, " "); //Определение второго операнда

										if (instruction == NULL) //Если второй операнд отсутствует
										{
											//Выдать ошибку и выйти из функции
											cerr << "Строка " << numStr << ": Отсутствует выражение после логического оператора." << endl;
											system("pause");
											return 1;
										}
										else if (strlen(instruction) == 1) //Если второй операнд присутствует и его длина - один символ
										{
											if (!isVar(instruction[0]) && !(instruction[0] >= 48 && instruction[0] <= 57)) //Если символ не перменная и не цифра
											{
												//Выдать ошибку и выйти из функции
												cerr << "Строка " << numStr << ": \"" << instruction << "\" не разрешен для использования в операторе \"if\"." << endl;
												system("pause");
												return 1;
											}
										}
										else if (strlen(instruction) > 1) //Если длина второго операнда более одного символа
										{
											for (int i = 0; i < strlen(instruction); i++) //Проверить каждый символ
											{
												if (!(instruction[i] >= 48 && instruction[i] <= 57)) //Если символ не соответствует цифре
												{
													//Выдать ошибку и выйти из функции
													cerr << "Строка " << numStr << ": \"" << instruction << "\" не разрешен для использования в операторе \"if\"." << endl;
													system("pause");
													return 1;
												}
											}
										}
									}
									else //Это не логический оператор
									{
										//Выдать ошибку и выйти из функции
										cerr << "Строка " << numStr << ": Неизвестный логический оператор \"" << instruction << "\"." << endl;
										system("pause");
										return 1;
									}
								}
							}
							else if (isVar(instruction[0]) || isZero(instruction[0])) //Первый операнд - переменная или цифра 0
							{
								instruction = strtok(NULL, " "); //Определить логический оператор

								if (instruction == NULL) //Если логический операнд отсутствует
								{
									//Выдать ошибку и выйти из функции
									cerr << "Строка " << numStr << ": Отсутствует логический оператор." << endl;
									system("pause");
									return 1;
								}
								else //Если логический операнд присутствует
								{

									if (isLogical(instruction)) //Это логический оператор
									{
										instruction = strtok(NULL, " "); //Определение второго операнда

										if (instruction == NULL) //Если второй операнд отсутствует
										{
											//Выдать ошибку и выйти из функции
											cerr << "Строка " << numStr << ": Отсутствует выражение после логического оператора." << endl;
											system("pause");
											return 1;
										}
										else if (strlen(instruction) == 1) //Если второй операнд присутствует и его длина - один символ
										{
											if (!isVar(instruction[0]) && !(instruction[0] >= 48 && instruction[0] <= 57)) //Если символ не перменная и не цифра
											{
												//Выдать ошибку и выйти из функции
												cerr << "Строка " << numStr << ": \"" << instruction << "\" не разрешен для использования в операторе \"if\"." << endl;
												system("pause");
												return 1;
											}
										}
										else if (strlen(instruction) > 1) //Если длина второго операнда более одного символа
										{
											for (int i = 0; i < strlen(instruction); i++) //Проверить каждый символ
											{
												if (!(instruction[i] >= 48 && instruction[i] <= 57)) //Если символ не соответствует цифре
												{
													//Выдать ошибку и выйти из функции
													cerr << "Строка " << numStr << ": \"" << instruction << "\" не разрешен для использования в операторе \"if\"." << endl;
													system("pause");
													return 1;
												}
											}
										}
									}
									else //Это не логический оператор
									{
										//Выдать ошибку и выйти из функции
										cerr << "Строка " << numStr << ": Неизвестный логический оператор \"" << instruction << "\"." << endl;
										system("pause");
										return 1;
									}
								}
							}
							else //Если встречени неизвестный символ
							{
								//Выдать ошибку и выйти из функции
								cerr << "Строка " << numStr << ": \"" << instruction << "\" не разрешен для использования в операторе \"if\"." << endl;
								system("pause");
								return 1;
							}
						}
						else if (strlen(instruction) > 1) //Если длина лексемы более одного символа
						{
							for (int i = 0; i < strlen(instruction); i++) //Проверить каждый символ
							{
								if (!(instruction[i] >= 48 && instruction[i] <= 57)) //Если символ не перменная и не цифра
								{
									//Выдать ошибку и выйти из функции
									cerr << "Строка " << numStr << ": \"" << instruction << "\" не разрешен для использования в операторе \"if\"." << endl;
									system("pause");
									return 1;
								}
							}

							instruction = strtok(NULL, " "); //Определить логический оператор

							if (instruction == NULL) //Если логический операнд отсутствует
							{
								//Выдать ошибку и выйти из функции
								cerr << "Строка " << numStr << ": Отсутствует логический оператор." << endl;
								system("pause");
								return 1;
							}
							else //Если логический операнд присутствует
							{
								if (isLogical(instruction)) //Это логический оператор
								{
									instruction = strtok(NULL, " "); //Определение второго операнда

									if (instruction == NULL) //Если второй операнд отсутствует
									{
										//Выдать ошибку и выйти из функции
										cerr << "Строка " << numStr << ": Отсутствует выражение после логического оператора." << endl;
										system("pause");
										return 1;
									}
									else if (strlen(instruction) == 1) //Если второй операнд присутствует и его длина - один символ
									{
										if (!isVar(instruction[0]) && !(instruction[0] >= 48 && instruction[0] <= 57)) //Если символ не перменная и не цифра
										{
											//Выдать ошибку и выйти из функции
											cerr << "Строка " << numStr << ": \"" << instruction << "\" не разрешен для использования в операторе \"if\"." << endl;
											system("pause");
											return 1;
										}
									}
									else if (strlen(instruction) > 1) //Если длина второго операнда более одного символа
									{
										for (int i = 0; i < strlen(instruction); i++) //Проверить каждый символ
										{
											if (!(instruction[i] >= 48 && instruction[i] <= 57)) //Если символ не соответствует цифре
											{
												//Выдать ошибку и выйти из функции
												cerr << "Строка " << numStr << ": \"" << instruction << "\" не разрешен для использования в операторе \"if\"." << endl;
												system("pause");
												return 1;
											}
										}
									}
								}
								else //Это не логический оператор
								{
									//Выдать ошибку и выйти из функции
									cerr << "Строка " << numStr << ": Неизвестный логический оператор \"" << instruction << "\"." << endl;
									system("pause");
									return 1;
								}
							}
						}
						else //Если встречена неизвестная лексема
						{
							//Выдать ошибку и выйти из функции
							cerr << "Строка " << numStr << ": \"" << instruction << "\" не разрешен для использования в операторе \"if\"." << endl;
							system("pause");
							return 1;
						}
					}
					
					instruction = strtok(NULL, " "); //Считать следующую лексему, чтобы встретить "goto"

					if (instruction == NULL) //Если оператор "goto" отсутствует
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Отсутствует оператор \"goto\" после логического выражения." << endl;
						system("pause");
						return 1;
					}
					else continue; //Если оператор "goto" присутствует - начать новый шаг цикла по определению лексем
				}
				else //Если встречена неизвестная лексема
				{
					//Выдать ошибку и выйти из функции
					cerr << "Строка " << numStr << ": Неизвестный оператор \"" << instruction << "\"." << endl;
					system("pause");
					return 1;
				}
			}
			else if (strlen(instruction) == 6) //Если длина лексемы - шесть символов
			{
				if (strcmp(instruction, "return") == 0) //Если лексема  - оператор "return"
				{
					strWhenSubBegin = 0; //Обнулить значение строки, с которой начато объявление функции
				}
				else //Если встречена неизвестная лексема
				{
					//Выдать ошибку и выйти из функции
					cerr << "Строка " << numStr << ": Неизвестный оператор \"" << instruction << "\"." << endl;
					system("pause");
					return 1;
				}
			}
			else if (strlen(instruction) == 5) //Если длина лексемы - пять символов
			{
				if (strcmp(instruction, "input") == 0) //Если лексема  - оператор "input"
				{
					instruction = strtok(NULL, " "); //Определение переменных 

					if (instruction == NULL) //Если переменная отсутствует
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Отсутствует переменная после оператора \"input\"." << endl;
						system("pause");
						return 1;
					}
					else //Если переменная присутствует
					{
						while (instruction != NULL) //Пока не прочитана вся строка
						{
							if (strlen(instruction) == 2 || strlen(instruction) == 1) //Если найдена переменная или одна из переменных (с запятой)
							{
								if (!isVar(instruction[0])) //Не найдена переменная для ввода значения
								{
									//Выдать ошибку и выйти из функции
									cerr << "Строка " << numStr << ": \"" << instruction << "\" не является переменной." << endl;
									system("pause");
									return 1;
								}
							}
							else //Лексема не удовлетворяет размеру переменной
							{
								//Выдать ошибку и выйти из функции
								cerr << "Строка " << numStr << ": \"" << instruction << "\" не является переменной." << endl;
								system("pause");
								return 1;
							}

							instruction = strtok(NULL, " "); //Считать следующую лексему
						}
					}
				}
				else if (strcmp(instruction, "print") == 0) //Если лексема  - оператор "print"
				{
					instruction = strtok(NULL, " "); //Определение переменных или строки

					if (instruction == NULL) //Если переменные или строка отсутствуют
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Отсутствуют данные для вывода после оператора \"print\"." << endl;
						system("pause");
						return 1;
					}
					else  //Если переменные или строка присутствуют
					{
						if (instruction[0] == '"') //Определено начало строки
						{
							if (strlen(instruction) == 1) //Лексема содержит только символ двойные кавычки
							{
								instruction = strtok(NULL, ""); //Считать всю оставшуюся строку

								if (instruction == NULL) //Если отсутствует продолжение строки
								{
									//Выдать ошибку и выйти из функции
									cerr << "Строка " << numStr << ": Отсутствует закрывающий символ \"." << endl;
									system("pause");
									return 1;
								}
							}
							else //Лексема содержит более одного символа
							{
								if (instruction[0] == '"' && instruction[1] == '"') //Отсутствуют символы в строке ("")
								{
									//Выдать ошибку и выйти из функции
									cerr << "Строка " << numStr << ": В строке отсутствуют символы." << endl;
									system("pause");
									return 1;
								}

								if (!(instruction[0] == '"' && instruction[strlen(instruction) - 1] == '"')) //Если строка не состоит из одного слова
								{
									instruction = strtok(NULL, "");  //Считать всю оставшуюся строку

									if (instruction == NULL) //Если отсутствует продолжение строки
									{
										//Выдать ошибку и выйти из функции
										cerr << "Строка " << numStr << ": Отсутствует закрывающий символ \"." << endl;
										system("pause");
										return 1;
									}
									else if (instruction[strlen(instruction) - 1] != '"') //Отсутствует символ двойные кавычки
									{
										//Выдать ошибку и выйти из функции
										cerr << "Строка " << numStr << ": Отсутствует закрывающий символ \"." << endl;
										system("pause");
										return 1;
									}
								}
							}
						}
						else //Определена переменная
						{
							while (instruction != NULL) //Пока не прочитана вся строка
							{
								if (strlen(instruction) == 2 || strlen(instruction) == 1) //Если найдена переменная или одна из переменных (с запятой)
								{
									if (!isVar(instruction[0])) //Не найдена переменная для вывода
									{
										//Выдать ошибку и выйти из функции
										cerr << "Строка " << numStr << ": \"" << instruction << "\" не является переменной." << endl;
										system("pause");
										return 1;
									}
								}
								else //Лексема не удовлетворяет размеру переменной
								{
									//Выдать ошибку и выйти из функции
									cerr << "Строка " << numStr << ": \"" << instruction << "\" не является переменной." << endl;
									system("pause");
									return 1;
								}

								instruction = strtok(NULL, " "); //Считать следующую лексему
							}
						}

						instruction = strtok(NULL, " "); //Считать следующую лексему
					}
				}
				else if (strcmp(instruction, "gosub") == 0) //Если лексема  - оператор "gosub"
				{
					instruction = strtok(NULL, " "); //Считать название функции

					if (instruction == NULL) //Имя функции не указано
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Требуется имя функции после оператора \"gosub\"." << endl;
						system("pause");
						return 1;
					}
					else //Проверить найденное имя функции
					{
						if (atoi(instruction) != 0 || isZero(instruction[0])) //Если имя функции начинается с цифры
						{
							//Выдать ошибку и выйти из функции
							cerr << "Строка " << numStr << ": Имя функции не должно начинаться с цифр." << endl;
							system("pause");
							return 1;
						}
						else
						{
							for (int i = 0; i < strlen(instruction); i++) //Проверить каждый символ в имени функции
							{
								if (!(instruction[i] >= 65 && instruction[i] <= 90) && !(instruction[i] >= 97 && instruction[i] <= 122) && !(instruction[i] >= 48 && instruction[i] <= 57)) //Если символ не соответствует букве или цифре
								{
									//Выдать ошибку и выйти из функции
									cerr << "Строка " << numStr << ": Имя функции содержит символы отличные от \"a-z\", \"A-Z\" или \"0-9\"." << endl;
									system("pause");
									return 1;
								}
							}
						}

						//Если ошибки в имени функции отсутствуют

						char *func_name = new char[strlen(instruction)]; //Массив хранит имя функции

						strcpy(func_name, instruction); //Сохранить ммя функции

						if ((instruction = strtok(NULL, " ")) != NULL) //Если после имени функции не встречен конец строки
						{
							//Выдать ошибку и выйти из функции
							cerr << "Строка " << numStr << ": Строка \"" << instruction << "\" не должна использоваться после имени функции." << endl;
							system("pause");
							return 1;
						}

						std::ios::pos_type pos = inputFile.tellg(); //Сохранить текущую позицию считывания из файла

						inputFile.seekg(0); //Начать чтение с начала файла

						inputFile.getline(buffer, BUFF_SIZE); //Считать строку из файла

						instruction = strtok(buffer, " "); //Прочитать первую лексему

						while (strcmp(instruction, func_name) != 0 && !inputFile.eof()) //Пока не встречено объявление нужной функции или не достигнут конец файла
						{
							if (inputFile.tellg() == pos) //Если строка является строкой вызова функции - перейти к следующей строке
							{
								inputFile.getline(buffer, BUFF_SIZE); //Считать строку из файла

								instruction = strtok(buffer, " "); //Прочитать первую лексему

								continue; //Новый шаг выполнения цикла
							}

							instruction = strtok(NULL, " "); //Считать лексему

							if (instruction == NULL) //Если прочитана вся строка
							{
								inputFile.getline(buffer, BUFF_SIZE); //Считать строку из файла

								instruction = strtok(buffer, " "); //Прочитать первую лексему
							}
						}

						if (inputFile.eof()) //Если функция не была найдена
						{
							//Выдать ошибку и выйти из функции
							cerr << "Функция \"" << func_name << "\" не найдена." << endl;
							system("pause");
							return 1;
						}

						inputFile.seekg(pos); //Вернуться к требуемой позиции для дальнейшего считывания файла

						delete[] func_name; //Высвободить память
					}
				}
				else //Если встречена неизвестная лексема
				{
					//Выдать ошибку и выйти из функции
					cerr << "Строка " << numStr << ": Неизвестный оператор \"" << instruction << "\"." << endl;
					system("pause");
					return 1;
				}
			}
			else if (strlen(instruction) == 4) //Если длина лексемы - четыре символа
			{
				if (strcmp(instruction, "goto") == 0) //Если лексема  - оператор "goto"
				{
					instruction = strtok(NULL, " "); //Определить адрес перехода

					if (instruction == NULL) //Если адрес перехода отсутствует
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Отсутствует адрес перехода после оператора \"goto\"." << endl;
						system("pause");
						return 1;
					}

					if (atoi(instruction) == 0) //Если адрес перехода не является номером строки
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Адресом перехода может служить только номер строки." << endl;
						system("pause");
						return 1;
					}

					int length = strlen(instruction) + 1;

					char *branch_address = new char[length]; //Массив хранит адрес перехода

					strcpy(branch_address, instruction); //Сохранить адрес перехода

					std::ios::pos_type pos = inputFile.tellg(); //Сохранить текущую позицию считывания из файла

					inputFile.seekg(0); //Начать чтение с начала файла
					
					if ((instruction = strtok(NULL, " ")) != NULL) //Если после адреса перехода не встречен конец строки
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Превышено количество адресов перехода после оператора \"goto\"." << endl;
						system("pause");
						delete[] branch_address; //Высвободить память
						return 1;
					}

					inputFile.getline(buffer, BUFF_SIZE); //Считать строку из файла
					 
					instruction = strtok(buffer, " "); //Прочитать первую лексему

					while (strcmp(branch_address, instruction) != 0 && !inputFile.eof()) //Пока не встречена строка перехода или не достигнут конец файла
					{
						inputFile.getline(buffer, BUFF_SIZE); //Считать строку из файла

						instruction = strtok(buffer, " "); //Прочитать первую лексему
					}

					if (inputFile.eof()) //Если строка не была найдена
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Адрес строки, указанный после оператора \"goto\" не найден." << endl;
						system("pause");
						delete[] branch_address; //Высвободить память
						return 1;
					}

					inputFile.seekg(pos); //Вернуться к требуемой позиции для дальнейшего считывания файла

					delete[] branch_address; //Высвободить память

					break; //Выйти из цикла по считыванию строки
				}
				else if (strcmp(instruction, "next") == 0) //Если лексема  - оператор "next"
				{
					countUnfinishedFor--; //Уменьшить колдичество незакрытых циклов "for" 

					if (countUnfinishedFor < 0) //Если количество незакрытых циклов "for" меньше нуля
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Отсутствует цикл \"for\" перед оператором \"next\"." << endl;
						system("pause");
						return 1;
					}
				}
				else //Если встречена неизвестная лексема
				{
					//Выдать ошибку и выйти из функции
					cerr << "Строка " << numStr << ": Неизвестный оператор \"" << instruction << "\"." << endl;
					system("pause");
					return 1;
				}
			}
			else if (strlen(instruction) == 3) //Если длина лексемы - три символа
			{
				if (strcmp(instruction, "rem") == 0) //Если лексема  - оператор "rem"
				{
					break; //Переход на следующую строку
				}
				else if (strcmp(instruction, "let") == 0) //Если лексема  - оператор "let"
				{
					instruction = strtok(NULL, " "); //Считать переменную, хранящую результат выражения

					if (instruction == NULL) //Если переменная отсутствует
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Требуется выражение после оператора \"let\"." << endl;
						system("pause");
						return 1;
					}
					
					if (strlen(instruction) == 1) //Если длина лексемы - один символ
					{
						if (isVar(instruction[0])) //Если символ - перменнная
						{
							instruction = strtok(NULL, " "); //Считать знак '='

							if (instruction == NULL) //Если лексема пуста
							{
								//Выдать ошибку и выйти из функции
								cerr << "Строка " << numStr << ": Переменной не присвоено выражение." << endl;
								system("pause");
								return 1;
							}
							else //Если лексема не пуста
							{
								if (strlen(instruction) == 1) //Если длина лексемы - один символ
								{
									if (instruction[0] == '=') //Если символ - знак '='
									{
										instruction = strtok(NULL, " "); //Считать первую лексему из выражения

										if (instruction == NULL) //Если лексема пуста
										{
											//Выдать ошибку и выйти из функции
											cerr << "Строка " << numStr << ": Требуется выражение после \"=\"." << endl;
											system("pause");
											return 1;
										}
										else //Если лексема не пуста - продолжить считывание выражения
										{
											int couplesOfbrackets = 0; //Проверяет, остались ли в выражении незакрытые скобки
											bool hasFirstOperand = false; //Проверяет, указан ли первый операнд перед прочитанным арифметическим знаком или переменной
											bool hasOperator = false; //Проверяет, указан ли арифметический оператор
											
											while (instruction != NULL) //Пока не прочитана вся строка
											{
												if (strlen(instruction) == 1) //Если длина лексемы - один символ
												{
													if (isVar(instruction[0])) //Лексема является переменной
													{
														if (hasFirstOperand == false && hasOperator == false) //Переменная является первым операндом в выражении
														{
															hasFirstOperand = true; //Указать, что инициализирован первый операнд
														}
														else if (hasFirstOperand == true && hasOperator == true) //Переменная является вторым операндом в выражении
														{
															hasOperator = false; //Выражение составлено. Установить значение false для hasOperator, чтобы воторой операнд стал первым в возможном следующем арифметичнском выражении 
														}
														else if (hasFirstOperand == true && hasOperator == false) //Переменная следует за первым операндом
														{
															//Выдать ошибку и выйти из функции
															cerr << "Строка " << numStr << ": Требуется арифметический оператор перед \'" << instruction[0] << "\'." << endl;
															system("pause");
															return 1;
														}

														instruction = strtok(NULL, " "); //Прочитать следующую лексему
														continue; //Начать новый шаг цикла
													}
													else if (atoi(instruction) != 0 || isZero(instruction[0])) //Лексема является однозначным числом
													{
														if (hasFirstOperand == false && hasOperator == false) //Число является первым операндом в выражении
														{
															hasFirstOperand = true; //Указать, что инициализирован первый операнд
														}
														else if (hasFirstOperand == true && hasOperator == true) //Число является вторым операндом в выражении
														{
															hasOperator = false; //Выражение составлено. Установить значение false для hasOperator, чтобы воторой операнд стал первым в возможном следующем арифметичнском выражении
														}
														else if (hasFirstOperand == true && hasOperator == false) //Число следует за первым операндом
														{
															//Выдать ошибку и выйти из функции
															cerr << "Строка " << numStr << ": Требуется арифметический оператор перед \'" << instruction[0] << "\'." << endl;
															system("pause");
															return 1;
														}

														instruction = strtok(NULL, " "); //Прочитать следующую лексему
														continue; //Начать новый шаг цикла
													}
													else if (isOperator(instruction[0]) && instruction[0] != '=') //Лексема является арифметическим оператором
													{
														if (hasFirstOperand == false) //Если отсутствует первый операнд перед арифметическим оператором
														{
															//Выдать ошибку и выйти из функции
															cerr << "Строка " << numStr << ": Требуется операнд перед \'" << instruction[0] << "\'." << endl;
															system("pause");
															return 1;
														}
														else if (hasOperator == true) //Оператор следует за другим оператором
														{
															//Выдать ошибку и выйти из функции
															cerr << "Строка " << numStr << ": \'" << instruction[0] << "\' не является операндом." << endl;
															system("pause");
															return 1;
														}

														instruction = strtok(NULL, " "); //Считать следующую лексему

														if (instruction == NULL) //Отсутствует второй операнд
														{
															//Выдать ошибку и выйти из функции
															cerr << "Строка " << numStr << ": Отсутствует операнд после арифметического оператора." << endl;
															system("pause");
															return 1;
														}
														else //Найден возможный второй операнд
														{
															hasOperator = true; //Определен арифметический оператор в выражении
														}
													}
													else //Символ не может использоваться в арифметическом выражении
													{
														//Выдать ошибку и выйти из функции
														cerr << "Строка " << numStr << ": Символ \'" << instruction[0] << "\' не разрешен для использования в выражении." << endl;
														system("pause");
														return 1;
													}
												}
												else //Если длина лексемы больше одного символа
												{
													int length = strlen(instruction); //Сохранить длину лексемы

													if (instruction[0] == '(') //Сначала идет открывающая скобка вида "(x"
													{
														couplesOfbrackets++; //Увеличить количество незакрытых скобок

														if (instruction[length - 1] == ')') //Внутри скобки находится только число или переменная вида "(x)"
														{
															couplesOfbrackets--; //Уменьшить количество незакрытых скобок

															if (length == 2) //Если скобки пустые
															{
																//Выдать ошибку и выйти из функции
																cerr << "Строка " << numStr << ": Отсутствует выражение между \'(\' и \')\'." << endl;
																system("pause");
																return 1;
															}
															else //Если скобки не пустые - скопировать содержимое
															{
																char *content = new char[length - 2]; //Массив хранит содержимое внутри скобок

																for (int i = 1; i < length - 1; i++) //Скопировать лексему в массив
																{
																	content[i - 1] = instruction[i];
																}

																bool nextIsDuplicateSign = false; //Определяет, имеется ли повторяющий арифметический оператор вида "++"
																bool nextIsDuplicateVar = false; //Определяет, имеется ли повторяющаяся переменная вида "xx" или "xz"
																bool varWasDefined = false; //Определяет, была ли прочитана переменная в выражении
																bool digitWasDefined = false; //Определяет, была ли прочитана цифра в выражении

																for (int i = 0; i < strlen(content); i++) //Проверить каждый символ
																{
																	if (content[i] == '-' || content[i] == '+') //Если символ является знаком '+' или '-', определяющие отрицательность числа
																	{
																		if (nextIsDuplicateSign) //Если данный символ встречается еще раз
																		{
																			//Выдать ошибку и выйти из функции
																			cerr << "Строка " << numStr << ": Встречен повторяющийся символ \'" << content[i] << "\' между \'(\' и \')\'." << endl;
																			system("pause");
																			delete[] content; //Высвободить память
																			return 1;
																		}
																		else if (digitWasDefined) //Если перед знаком имеется цифра вида "1+"
																		{
																			//Выдать ошибку и выйти из функции
																			cerr << "Строка " << numStr << ": Некорректное значение \"" << content << "\"." << endl;
																			system("pause");
																			delete[] content; //Высвободить память
																			return 1;
																		}
																		else
																		{
																			nextIsDuplicateSign = true; //Следующий такой же символ вызовет ошибку
																		}
																	}
																	else if (isVar(content[i])) //Если символ является переменной
																	{
																		if (nextIsDuplicateVar) //Повторяющееся имя переменной вида "xx" или "xz"
																		{
																			//Выдать ошибку и выйти из функции
																			cerr << "Строка " << numStr << ": Встречена повторяющаяся переменная \'" << content[i] << "\' между \'(\' и \')\'." << endl;
																			system("pause");
																			delete[] content; //Высвободить память
																			return 1;
																		}
																		else if (digitWasDefined) //До переменной идет цифра вида "1y"
																		{
																			//Выдать ошибку и выйти из функции
																			cerr << "Строка " << numStr << ": Неправильное имя переменной \"" << content[i - 1] << content[i] << "\"." << endl;
																			system("pause");
																			delete[] content; //Высвободить память
																			return 1;
																		}
																		else
																		{
																			varWasDefined = true; //Переменная прочитана
																			nextIsDuplicateVar = true; //Следующий символ не должен быть переменной
																		}
																	}
																	else if (content[i] >= 48 && content[i] <= 57) //Если символ является цифрой
																	{
																		if (varWasDefined) //После переменной идет цифра вида "x2"
																		{
																			//Выдать ошибку и выйти из функции
																			cerr << "Строка " << numStr << ": Неправильное имя переменной \"" << content[i - 1] << content[i] << "\"." << endl;
																			system("pause");
																			delete[] content; //Высвободить память
																			return 1;
																		}
																		else
																		{
																			digitWasDefined = true; //Указать, что была прочитана цифра
																		}
																	}
																	else //Если символ неизвестен
																	{
																		//Выдать ошибку и выйти из функции
																		cerr << "Строка " << numStr << ": Неразрешенный символ \'" << content[i] << "\'." << endl;
																		system("pause");
																		delete[] content; //Высвободить память
																		return 1;
																	}
																}

																delete[] content; //Высвободить память
															}
														}
														else //Внутри скобки - выражение
														{
															/*----------------- Проверка соответствия содержимого с '(' в начале -------------*/

															bool nextIsDuplicateSign = false; //Определяет, имеется ли повторяющий арифметический оператор вида "++"
															bool nextIsDuplicateVar = false; //Определяет, имеется ли повторяющаяся переменная вида "xx" или "xz"
															bool varWasDefined = false; //Определяет, была ли прочитана переменная в выражении
															bool digitWasDefined = false; //Определяет, была ли прочитана цифра в выражении

															for (int i = 1; i < length; i++) //Проверить каждый символ кроме '(' в начале
															{
																if (instruction[i] == '-' || instruction[i] == '+') //Если символ является знаком '+' или '-', определяющие отрицательность числа
																{
																	if (nextIsDuplicateSign) //Если данный символ встречается еще раз
																	{
																		//Выдать ошибку и выйти из функции
																		cerr << "Строка " << numStr << ": Встречен повторяющийся символ \'" << instruction[i] << "\' между \'(\' и \')\'." << endl;
																		system("pause");
																		return 1;
																	}
																	else if (digitWasDefined) //Если перед знаком имеется цифра вида "1+"
																	{
																		//Выдать ошибку и выйти из функции
																		cerr << "Строка " << numStr << ": Некорректное значение \"" << instruction << "\"." << endl;
																		system("pause");
																		return 1;
																	}
																	else
																	{
																		nextIsDuplicateSign = true; //Следующий такой же символ вызовет ошибку
																	}
																}
																else if (isVar(instruction[i])) //Если символ является переменной
																{
																	if (nextIsDuplicateVar) //Повторяющееся имя переменной вида "xx" или "xz"
																	{
																		//Выдать ошибку и выйти из функции
																		cerr << "Строка " << numStr << ": Встречена повторяющаяся переменная \'" << instruction[i] << "\' между \'(\' и \')\'." << endl;
																		system("pause");
																		return 1;
																	}
																	else if (digitWasDefined) //До переменной идет цифра вида "1y"
																	{
																		//Выдать ошибку и выйти из функции
																		cerr << "Строка " << numStr << ": Неправильное имя переменной \"" << instruction[i - 1] << instruction[i] << "\"." << endl;
																		system("pause");
																		return 1;
																	}
																	else
																	{
																		varWasDefined = true; //Переменная прочитана
																		nextIsDuplicateVar = true; //Следующий символ не должен быть переменной
																	}
																}
																else if (instruction[i] >= 48 && instruction[i] <= 57) //Если символ является цифрой
																{
																	if (varWasDefined) //После переменной идет цифра вида "x2"
																	{
																		//Выдать ошибку и выйти из функции
																		cerr << "Строка " << numStr << ": Неправильное имя переменной \"" << instruction[i - 1] << instruction[i] << "\"." << endl;
																		system("pause");
																		return 1;
																	}
																	else
																	{
																		digitWasDefined = true; //Указать, что была прочитана цифра
																	}
																}
																else //Если символ неизвестен
																{
																	//Выдать ошибку и выйти из функции
																	cerr << "Строка " << numStr << ": Неразрешенный символ \'" << instruction[i] << "\'." << endl;
																	system("pause");
																	return 1;
																}
															}

															/*------------------ Окончание Проверки ------------------------*/

															hasFirstOperand = true; //Указать, что инициализирован первый операнд
															hasOperator = false; //Выражение составлено. Установить значение false для hasOperator, чтобы воторой операнд стал первым в возможном следующем арифметичнском выражении

															instruction = strtok(NULL, " "); //Продолжить чтение выражения
															continue; //Начать новый шаг цикла
														}
													}
													else if (instruction[length - 1] == ')') //Имеется закрывающая скобка вида "x)"
													{
														couplesOfbrackets--; //Уменьшить количество незакрытых скобок

														/*----------------- Проверка соответствия содержимого с ')' в конце -------------*/

														bool nextIsDuplicateSign = false; //Определяет, имеется ли повторяющий арифметический оператор вида "++"
														bool nextIsDuplicateVar = false; //Определяет, имеется ли повторяющаяся переменная вида "xx" или "xz"
														bool varWasDefined = false; //Определяет, была ли прочитана переменная в выражении
														bool digitWasDefined = false; //Определяет, была ли прочитана цифра в выражении

														for (int i = 0; i < length - 1; i++)
														{
															if (instruction[i] == '-' || instruction[i] == '+') //Проверить каждый символ
															{
																if (nextIsDuplicateSign) //Если данный символ встречается еще раз 
																{
																	//Выдать ошибку и выйти из функции
																	cerr << "Строка " << numStr << ": Встречен повторяющийся символ \'" << instruction[i] << "\' между \'(\' и \')\'." << endl;
																	system("pause");
																	return 1;
																}
																else if (digitWasDefined) //Если перед знаком имеется цифра вида "1+"
																{
																	//Выдать ошибку и выйти из функции
																	cerr << "Строка " << numStr << ": Некорректное значение \"" << instruction << "\"." << endl;
																	system("pause");
																	return 1;
																}
																else
																{
																	nextIsDuplicateSign = true; //Следующий такой же символ вызовет ошибку
																}
															}
															else if (isVar(instruction[i])) //Если символ является переменной
															{
																if (nextIsDuplicateVar) //Повторяющееся имя переменной вида "xx" или "xz"
																{
																	//Выдать ошибку и выйти из функции
																	cerr << "Строка " << numStr << ": Встречена повторяющаяся переменная \'" << instruction[i] << "\' между \'(\' и \')\'." << endl;
																	system("pause");
																	return 1;
																}
																else if (digitWasDefined) //До переменной идет цифра вида "1y"
																{
																	//Выдать ошибку и выйти из функции
																	cerr << "Строка " << numStr << ": Неправильное имя переменной \"" << instruction[i - 1] << instruction[i] << "\"." << endl;
																	system("pause");
																	return 1;
																}
																else
																{
																	varWasDefined = true; //Переменная прочитана
																	nextIsDuplicateVar = true; //Следующий символ не должен быть переменной
																}
															}
															else if (instruction[i] >= 48 && instruction[i] <= 57) //Если символ является цифрой
															{
																if (varWasDefined) //После переменной идет цифра вида "x2"
																{
																	//Выдать ошибку и выйти из функции
																	cerr << "Строка " << numStr << ": Неправильное имя переменной \"" << instruction[i - 1] << instruction[i] << "\"." << endl;
																	system("pause");
																	return 1;
																}
																else
																{
																	digitWasDefined = true; //Указать, что была прочитана цифра
																}
															}
															else //Если символ неизвестен
															{
																//Выдать ошибку и выйти из функции
																cerr << "Строка " << numStr << ": Неразрешенный символ \'" << instruction[i] << "\'." << endl;
																system("pause");
																return 1;
															}
														}

														/*------------------ Окончание Проверки ------------------------*/
													}
													else //Имеется некоторая строка
													{
														/*----------------- Проверка соответствия содержимого -------------*/

														bool nextIsDuplicateSign = false; //Определяет, имеется ли повторяющий арифметический оператор вида "++"
														bool nextIsDuplicateVar = false; //Определяет, имеется ли повторяющаяся переменная вида "xx" или "xz"
														bool varWasDefined = false; //Определяет, была ли прочитана переменная в выражении
														bool digitWasDefined = false; //Определяет, была ли прочитана цифра в выражении

														for (int i = 0; i < strlen(instruction); i++)
														{
															if (instruction[i] == '-' || instruction[i] == '+') //Проверить каждый символ
															{
																if (nextIsDuplicateSign) //Если данный символ встречается еще раз 
																{
																	//Выдать ошибку и выйти из функции
																	cerr << "Строка " << numStr << ": Встречен повторяющийся символ \'" << instruction[i] << "\' между \'(\' и \')\'." << endl;
																	system("pause");
																	return 1;
																}
																else if (digitWasDefined) //Если перед знаком имеется цифра вида "1+"
																{
																	//Выдать ошибку и выйти из функции
																	cerr << "Строка " << numStr << ": Некорректное значение \"" << instruction << "\"." << endl;
																	system("pause");
																	return 1;
																}
																else
																{
																	nextIsDuplicateSign = true; //Следующий такой же символ вызовет ошибку
																}
															}
															else if (isVar(instruction[i])) //Если символ является переменной
															{
																if (nextIsDuplicateVar)  //Повторяющееся имя переменной вида "xx" или "xz"
																{
																	//Выдать ошибку и выйти из функции
																	cerr << "Строка " << numStr << ": Встречена повторяющаяся переменная \'" << instruction[i] << "\' между \'(\' и \')\'." << endl;
																	system("pause");
																	return 1;
																}
																else if (digitWasDefined) //До переменной идет цифра вида "1y"
																{
																	//Выдать ошибку и выйти из функции
																	cerr << "Строка " << numStr << ": Неправильное имя переменной \"" << instruction[i - 1] << instruction[i] << "\"." << endl;
																	system("pause");
																	return 1;
																}
																else
																{
																	varWasDefined = true; //Переменная прочитана
																	nextIsDuplicateVar = true; //Следующий символ не должен быть переменной
																}
															}
															else if (instruction[i] >= 48 && instruction[i] <= 57) //Если символ является цифрой
															{
																if (varWasDefined) //После переменной идет цифра вида "x2"
																{
																	//Выдать ошибку и выйти из функции
																	cerr << "Строка " << numStr << ": Неправильное имя переменной \"" << instruction[i - 1] << instruction[i] << "\"." << endl;
																	system("pause");
																	return 1;
																}
																else
																{
																	digitWasDefined = true; //Указать, что была прочитана цифра
																}
															}
															else //Если символ неизвестен
															{
																//Выдать ошибку и выйти из функции
																cerr << "Строка " << numStr << ": Неразрешенный символ \'" << instruction[i] << "\'." << endl;
																system("pause");
																return 1;
															}
														}

														/*------------------ Окончание Проверки ------------------------*/
													}

													hasFirstOperand = true; //Указать, что инициализирован первый операнд
													hasOperator = false; //Выражение составлено. Установить значение false для hasOperator, чтобы воторой операнд стал первым в возможном следующем арифметичнском выражении

													instruction = strtok(NULL, " "); //Продолжить чтение выражения
													continue; //Начать новый шаг цикла;
												}
											}

											if (couplesOfbrackets > 0) //Если количество незакрытых скобок больше нуля
											{
												//Выдать ошибку и выйти из функции
												cerr << "Строка " << numStr << ": Отсутствует закрывающая скобка в выражении." << endl;
												system("pause");
												return 1;
											}
											else if (couplesOfbrackets < 0) //Если количество незакрытых скобок меньше нуля
											{
												//Выдать ошибку и выйти из функции
												cerr << "Строка " << numStr << ": Имеется лишняя закрывающая скобка в выражении." << endl;
												system("pause");
												return 1;
											}
										}
									}
									else //Если символ не является знаком '='
									{
										//Выдать ошибку и выйти из функции
										cerr << "Строка " << numStr << ": Ожидался \"=\" вместо \"" << instruction << "\"." << endl;
										system("pause");
										return 1;
									}
								}
								else //Если длина лексемы более одного символа
								{
									//Выдать ошибку и выйти из функции
									cerr << "Строка " << numStr << ": Ожидался \"=\" вместо \"" << instruction << "\"." << endl;
									system("pause");
									return 1;
								}
							}
						}
						else //Если символ не перменнная
						{
							//Выдать ошибку и выйти из функции
							cerr << "Строка " << numStr << ": Ожидалась переменная вместо \"" << instruction << "\"." << endl;
							system("pause");
							return 1;
						}
					}
					else //Если длина лексемы более одного символа
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Ожидалась переменная вместо \"" << instruction << "\"." << endl;
						system("pause");
						return 1;
					}
				}
				else if (strcmp(instruction, "end") == 0) //Если лексема  - оператор "end"
				{
					if (endWasFound == false) //Встречен первый оператор end
					{
						endWasFound = true; //Указать, что оператор "end" инициализирован в программе
					}
					else //Встречен еще один оператор end
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Оператор \"end\" может быть объявлен только один раз в программе." << endl;
						system("pause");
						return 1;
					}

					if ((instruction = strtok(NULL, " ")) != NULL) //Если после оператора end не встречен конец строки
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Оператор \"end\" не должен содержать символы после своего объявления." << endl;
						system("pause");
						return 1;
					}

					if (countUnfinishedFor > 0) //Если количество неполных циклов "for" больше 0
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Ожидался оператор \"next\" вместо оператора \"end\"." << endl;
						system("pause");
						return 1;
					}

					if (strWhenSubBegin != 0) //Если в переменной хранится номер строки объявления функции, которая не была закрыта оператором "return"
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Ожидался оператор \"return\" вместо оператора \"end\"." << endl;
						system("pause");
						return 1;
					}

					break; //Выйти из цикла по считыванию строки
				}
				else if (strcmp(instruction, "sub") == 0) //Если лексема  - оператор "sub"
				{
					instruction = strtok(NULL, " "); //Считать название функции

					if (instruction == NULL) //Имя функции не указано
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Требуется имя функции после оператора \"sub\"." << endl;
						system("pause");
						return 1;
					}
					else //Проверить найденное имя функции
					{
						if (strWhenSubBegin == 0) //Если номер строки еще не сохранен в переменной
						{
							strWhenSubBegin = numStr; //Сохранить номер строки
						}
						else //Если в перменной имеется другой номер строки
						{
							//Выдать ошибку и выйти из функции
							cerr << "Строка " << numStr << ": Требуется оператор \"return\"." << endl;
							system("pause");
							return 1;
						}

						if (atoi(instruction) != 0 || isZero(instruction[0])) //Если имя функции начинается с цифр
						{
							//Выдать ошибку и выйти из функции
							cerr << "Строка " << numStr << ": Имя функции не должно начинаться с цифр." << endl;
							system("pause");
							return 1;
						}
						else
						{
							for (int i = 0; i < strlen(instruction); i++) //Проверить каждый символ
							{
								//Если символ не является цифрой или латинской буквой
								if (!(instruction[i] >= 65 && instruction[i] <= 90) && !(instruction[i] >= 97 && instruction[i] <= 122) && !(instruction[i] >= 48 && instruction[i] <= 57))
								{
									//Выдать ошибку и выйти из функции
									cerr << "Строка " << numStr << ": Имя функции содержит символы отличные от \"a-z\", \"A-Z\" или \"0-9\"." << endl;
									system("pause");
									return 1;
								}
							}
						}
					}

					if ((instruction = strtok(NULL, " ")) != NULL) //Если после имени функции не встречен конец строки
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Строка \"" << instruction << "\" не должна использоваться после имени функции." << endl;
						system("pause");
						return 1;
					}
				}
				else if (strcmp(instruction, "for") == 0) //Если лексема  - оператор "for"
				{
					instruction = strtok(NULL, " "); //Считать параметр

					if (instruction == NULL) //Если параметр отсутствует
					{
						//Выдать ошибку и выйти из функции
						cerr << "Строка " << numStr << ": Не указан параметр после оператора \"for\"." << endl;
						system("pause");
						return 1;
					}
					else //Если параметр присутствует
					{
						countUnfinishedFor++; //Увеличить количество незавершенных циклов "for"

						if (strlen(instruction) == 1) //Если длина лексемы - один символ
						{
							if (isVar(instruction[0])) //Если лексема - переменная
							{
								instruction = strtok(NULL, " "); //Считать знак '='

								if (instruction == NULL) //Если лексема отсуствует
								{
									//Выдать ошибку и выйти из функции
									cerr << "Строка " << numStr << ": Отсутствует инициализация параметра в цикле \"for\"." << endl;
									system("pause");
									return 1;
								}
								else //Если лексема присутствует
								{
									if (instruction[0] == '=' && strlen(instruction) == 1) //Если лексема является знаком '='
									{
										instruction = strtok(NULL, " "); //Считать начальное значение

										if (instruction == NULL) //Если начальное значение отсутствует
										{
											//Выдать ошибку и выйти из функции
											cerr << "Строка " << numStr << ": Отсутствует инициализация параметра в цикле \"for\"." << endl;
											system("pause");
											return 1;
										}
										else //Если начальное значение присутствует
										{
											for (int i = 0; i < strlen(instruction); i++) //Проверить каждый символ
											{
												if (!(instruction[i] >= 48 && instruction[i] <= 57)) //Если символ не является цифрой
												{
													//Выдать ошибку и выйти из функции
													cerr << "Строка " << numStr << ": \"" << instruction << "\" не является числом." << endl;
													system("pause");
													return 1;
												}
											}

											instruction = strtok(NULL, " "); //Считать "to"

											if (instruction == NULL) //Если "to" отсутствует
											{
												//Выдать ошибку и выйти из функции
												cerr << "Строка " << numStr << ": Отсутствует оператор \"to\"." << endl;
												system("pause");
												return 1;
											}
											else if (strcmp(instruction, "to") != 0) //Если лексема не "to"
											{
												//Выдать ошибку и выйти из функции
												cerr << "Строка " << numStr << ": Ожидался оператор \"to\" вместо \"" << instruction << "\"." << endl;
												system("pause");
												return 1;
											}
											else //Если "to" присутствует
											{
												instruction = strtok(NULL, " "); //Считать конечное значение

												if (instruction == NULL) //Если конечное значение отсутствует
												{
													//Выдать ошибку и выйти из функции
													cerr << "Строка " << numStr << ": Отсутствует конечное значение параметра в цикле \"for\"." << endl;
													system("pause");
													return 1;
												}
												else //Если конечное значение присутствует
												{
													for (int i = 0; i < strlen(instruction); i++) //Проверить каждый символ
													{
														if (!(instruction[i] >= 48 && instruction[i] <= 57)) //Если символ не является цифрой
														{
															//Выдать ошибку и выйти из функции
															cerr << "Строка " << numStr << ": \"" << instruction << "\" не является числом." << endl;
															system("pause");
															return 1;
														}
													}

													instruction = strtok(NULL, " "); //Считать лексему

													if (instruction != NULL) //Если не встречен конец строки
													{
														if (strcmp(instruction, "step") != 0) //Если лексема не оператор "step"
														{
															//Выдать ошибку и выйти из функции
															cerr << "Строка " << numStr << ": Ожидался оператор \"step\" вместо \"" << instruction << "\"." << endl;
															system("pause");
															return 1;
														}
														else //Если лексема оператор "step"
														{
															instruction = strtok(NULL, " "); //Определить шаг цикла

															if (instruction != NULL) //Если шаг цикла присутствует
															{
																for (int i = 0; i < strlen(instruction); i++) //Проверить каждый символ
																{
																	if (!(instruction[i] >= 48 && instruction[i] <= 57)) //Символ не является цифрой
																	{
																		//Выдать ошибку и выйти из функции
																		cerr << "Строка " << numStr << ": \"" << instruction << "\" не является числом." << endl;
																		system("pause");
																		return 1;
																	}
																}
															}
															else //Если шаг цикла отсутствует
															{
																//Выдать ошибку и выйти из функции
																cerr << "Строка " << numStr << ": Не указан шаг цикла." << endl;
																system("pause");
																return 1;
															}
														}
													}
												}
											}
										}
									}
									else //Если лексема не '='
									{
										//Выдать ошибку и выйти из функции
										cerr << "Строка " << numStr << ": Ожидался оператор \'=\' вместо \"" << instruction << "\"." << endl;
										system("pause");
										return 1;
									}
								}
							}
							else //Если лексема не перменная
							{
								//Выдать ошибку и выйти из функции
								cerr << "Строка " << numStr << ": Ожидалась переменная вместо \"" << instruction << "\", в качестве параметра." << endl;
								system("pause");
								return 1;
							}
						}
						else //Если длина лексема более одного символа
						{
							//Выдать ошибку и выйти из функции
							cerr << "Строка " << numStr << ": Ожидалась переменная вместо \"" << instruction << "\", в качестве параметра." << endl;
							system("pause");
							return 1;
						}
					}
				}
				else //Если встречена неизвестная лексема
				{
					//Выдать ошибку и выйти из функции
					cerr << "Строка " << numStr << ": Неизвестный оператор \"" << instruction << "\"." << endl;
					system("pause");
					return 1;
				}
			}
			else //Если встречена неизвестная лексема
			{
				//Выдать ошибку и выйти из функции
				cerr << "Строка " << numStr << ": Неизвестный оператор \"" << instruction << "\"." << endl;
				system("pause");
				return 1;
			}

			instruction = strtok(NULL, " "); //Считать следующую лексему
		}
	}

	inputFile.seekg(0); //Переместить указатель считывания в начало файла
	return 0; //Успешное завершение функции
}

//Проверяет, является ли символ логическим оператором
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

//Проверяет, является ли символ именем переменной
bool isVar(char ch)
{
	return ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122));
}

//Проверяет, является ли символ нулем
bool isZero(char ch)
{
	return (ch == 48);
}