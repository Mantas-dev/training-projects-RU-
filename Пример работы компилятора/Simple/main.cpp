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

//Операции ввода/вывода
const int READ = 10; //Вводит слово с терминала в указанное место памяти
const int WRITE = 11; //Выводит на терминал слово из указанного адреса памяти

//Операции загрузки/выгрузки
const int LOAD = 20; //Помещает в аккумулятор слово из указанного адреса памяти
const int STORE = 21; //Выгружает слово из аккумулятора по указанному адресу памяти

//Арифметические операции
const int ADD = 30; //Выполняет сложение слова в аккумуляторе и слова из указанного места
					//в памяти (результат операции остается в аккумуляторе)
const int SUBTRACT = 31; //Вычитает из слова в аккумуляторе слово из указанного места в памяти
						 //(результат операции остается в аккумуляторе)
const int DIVIDE = 32; //Выполняет деление слова в аккумуляторе на слово из указанного места
					   //в памяти (результат операции остается в аккумуляторе)
const int MULTIPLY = 33; //Вычисляет произведение слова в аккумуляторе и слова из указанного
						 //места в памяти (результат операции остается в аккумуляторе)
const int MODULE = 34; //Вычисляет взятие по модулю слова в аккумуляторе и слова из указанного
						 //места в памяти (результат операции остается в аккумуляторе)
const int POW = 35; //Вычисляет возведение слова в аккумуляторе в степень слова из указанного
					   //места в памяти (результат операции остается в аккумуляторе)

//Операции передачи управления
const int BRANCH = 40; //Переход к указанному адресу памяти
const int BRANCHNEG = 41; //Переход к указанному адресу памяти, если в аккумуляторе находится отрицательное чило
const int BRANCHZERO = 42; // Переход к указанному адресу памяти, если в аккумуляторе находится ноль
const int HALT = 43; //Останов, выполняется при завершении программой своей работы

//Операция вывода строки
const int STROUT = 90; //Выводит строку на экран

//КОНСТАНТЫ
const int SIZE = 100; //Размер массива SML-инструкций

//МАССИВЫ
int memory[SIZE]; //Массив, хранящий SML-инструкции

//ПЕРЕМЕННЫЕ
int accumulator = 0; //Переменная-аккумулятор. Хранит числовое значение
int instructionCounter = 0; //Хранит количество SML-инструкций
int operationCode = 0, operand = 0; //operationCode - первые две цифры в SML-инструкции; operand - последние две цифры в SML-инструкции
int instructionRegister = 0; //Хранит текущую SML-инструкцию

//ФАЙЛЫ
ifstream inputFile("../Debug/temp_code.sml", ios_base::in); //Файл для считывания SML-инструкций
ofstream outputFile("../Debug/result.txt", ios_base::out); //Файл для сохранения результата выполнения программы

//ФУНКЦИИ
void exportIntoFile(ofstream &); //Записывает в файл требуемую информацию о результате выполнения

int main()
{
	setlocale(LC_ALL, "Russian");

	char buffer[SIZE]; //Массив хранит SML-инструкцию

	bool cycleOn = true; //Определяет, продолжать ли выполнение цикла по выполнению SML-инструкций 

	int inputWord = 0; //Хранит числовое представление SML-инструкции

	cout << "*** Симплтрон приветствует вас! ***\n" << endl;
	
	while (!inputFile.eof()) //Пока не достигнут конец файла
	{
		inputFile.getline(buffer, SIZE); //Прочитать SML-инструкцию из файла

		inputWord = atoi(buffer); //Сохранить инструкцию в виде числа

		memory[instructionCounter] = inputWord; //Записать SML-инструкцию в массив
		++instructionCounter; //Увеличить счетчик SML-инструкций в массиве
	}
	
	instructionCounter = 0; //Задать начальное значение для считывания инструкций из массива

	cout << "*** Загрузка программы завершена ***\n"
		<< "*** Начинаю выполнение программы ***" << endl;

	while (cycleOn) //Пока разрешено выполнение цикла
	{
		instructionRegister = memory[instructionCounter]; //Сохранить SML-инструкцию в переменную

		operationCode = instructionRegister / 100; //Сохранить первые две цифры в переменную
		operand = instructionRegister % 100; //Сохранить последние две цифры в переменную

		switch (operationCode) //Определить оператор по первым двум цифрам SML-инструкции
		{
		case READ: //Оператор считывания значения с терминала в память
			cout << "? ";
			cin >> memory[operand]; //Записать значение в массив
			++instructionCounter; //Перейти к следующей SML-инструкции
			break;
		case WRITE: //Оператор вывода значения из памяти на экран
			cout << showpos << setw(5) << setfill('0') << internal << memory[operand] << endl; //Вывести значение на экран
			outputFile << showpos << setw(5) << setfill('0') << internal << memory[operand] << endl; //Записать значение в файл
			cout << setfill(' '); //Сбросить оператор setfill('0')
			++instructionCounter; //Перейти к следующей SML-инструкции
			break;
		case LOAD: //Оператор загрузки в аккумулятор значения из массива
			accumulator = memory[operand]; //Сохранить значение в аккумуляторе
			++instructionCounter; //Перейти к следующей SML-инструкции
			break;
		case STORE: //Оператор выгрузки из аккумулятора значения в массив 
			memory[operand] = accumulator; //Сохранить значение в массиве
			accumulator = 0; //Обнулить аккумулятор
			++instructionCounter; //Перейти к следующей SML-инструкции
			break;
		case ADD: //Оператор сложения значения в аккумуляторе и значения в массиве
			accumulator += memory[operand]; //Сохранить результат сложения в аккумуляторе
			++instructionCounter; //Перейти к следующей SML-инструкции
			break;
		case SUBTRACT: //Оператор вычитания из значения в аккумуляторе значения в массиве
			accumulator -= memory[operand]; //Сохранить результат вычитания в аккумуляторе
			++instructionCounter; //Перейти к следующей SML-инструкции
			break;
		case DIVIDE: //Оператор деления значения в аккумуляторе на значение в массиве
			if (memory[operand] == 0) //Если значение в массиве - 0
			{
				//Вывод ошибки на экран и в файл
				cout << "*** Попытка деления на ноль ***\n"
					<< "*** Симплтрон аварийно завершил выполнение программы ***" << endl;

				outputFile << "*** Попытка деления на ноль ***\n"
					<< "*** Симплтрон аварийно завершил выполнение программы ***" << endl;

				cycleOn = false; //Запретить дальнейшее выполнение цикла
				break;
			}
			//Если значения в массиве не равно нулю
			accumulator /= memory[operand]; //Сохранить результат деления в аккумуляторе
			++instructionCounter; //Перейти к следующей SML-инструкции
			break;
		case MULTIPLY: //Оператор умножения значения в аккумуляторе на значение в массиве
			accumulator *= memory[operand]; //Сохранить результат умножения в аккумуляторе
			++instructionCounter; //Перейти к следующей SML-инструкции
			break;
		case MODULE: //Оператор взятия по модулю значения в аккумуляторе и значение в массиве (оператор "%")
			accumulator %= memory[operand]; //Сохранит результат взятия по модулю в аккумуляторе
			++instructionCounter; //Перейти к следующей SML-инструкции
			break;
		case POW: //Оператор возведения в степень. Значение в аккумуляторе - основание; значение в массиве - показатель степени
			accumulator = pow(accumulator, memory[operand]); //Сохранить результат возведения в степень в аккумуляторе
			++instructionCounter; //Перейти к следующей SML-инструкции
			break;
		case BRANCH: //Оператор перехода к SML-инструкции по ссылке 
			instructionCounter = operand; //Перейти к требуемой SML-инструкции
			break;
		case BRANCHNEG: //Оператор перехода к SML-инструкции по минусу
			if (accumulator < 0) //Если значение в аккумуляторе меньше нуля
				instructionCounter = operand; //Перейти к требуемой SML-инструкции
			else //Если значение в аккумуляторе больше либо равно нулю
			{
				++instructionCounter; //Перейти к следующей SML-инструкции
			}
			break;
		case BRANCHZERO: //Оператор перехода к SML-инструкции по нулю
			if (accumulator == 0) //Если значение в аккумуляторе равно нулю
				instructionCounter = operand; //Перейти к требуемой SML-инструкции
			else //Если значение в аккумуляторе не равно нулю
			{
				++instructionCounter; //Перейти к следующей SML-инструкции
			}
			break;
		case HALT: //Оператор останова. Завершает выполнение программы
			cout << "*** Симплтрон закончил свои вычисления ***" << endl;

			outputFile << "*** Симплтрон закончил свои вычисления ***" << endl;

			cycleOn = false; //Запретить дальнейшее выполнение цикла
			break;
		case STROUT: //Оператор вывода строки на экран
			int strEnd = operand; //Сохранить количество символов в строке в переменную
			++instructionCounter; //Перейти к следующей SML-инструкции
			for (int i = 1; i <= strEnd; i++) //Вывести каждый символ на экран
			{
				instructionRegister = memory[instructionCounter]; //Сохранить SML-инструкцию в переменную

				operand = instructionRegister % 1000; //Определить ASCII-представление символа

				cout << char(operand); //Вывести символ

				++instructionCounter; //Перейти к следующей SML-инструкции
			}
			cout << endl;
			++instructionCounter; //Перейти к следующей SML-инструкции
			break;
		}

	}
	
	//Вывести на экран результат работы программы и дамп памяти

	cout << "РЕГИСТРЫ:\n"
		<< "accumulator    \t\t " << showpos << setw(5) << setfill('0') << internal << accumulator << "\n"
		<< "instructionCounter    \t    " << setw(2) << setfill('0') << noshowpos << internal << instructionCounter << "\n"
		<< "instructionRegister    \t " << setw(5) << setfill('0') << showpos << internal << instructionRegister << "\n"
		<< "operationCode    \t    " << setw(2) << setfill('0') << noshowpos << operationCode << "\n"
		<< "operand        \t\t    " << setw(2) << setfill('0') << noshowpos << internal << operand << "\n\n"
		<< "ПАМЯТЬ:" << endl;

	//Вывести столбцы
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
	
	for (int row = 0; row < 91; row += 10) //Вывод значения каждого элемента массива в виде таблицы
	{
		cout << noshowpos << setw(2) << row << " "; //Вывести номер строки
		for (int i = row; i < row + 10; i++)
		{
			cout << internal << showpos << setw(5) << setfill('0') << memory[i] << " "; //Вывести значение
		}
		cout << endl;
	}

	exportIntoFile(outputFile); //Записать результат работы программы в файл

	system("pause");

	return 0;
}

void exportIntoFile(ofstream &file) //Записывает результат выполнения программы в файл
{
	file << "РЕГИСТРЫ:\n"
		<< "accumulator    \t\t " << showpos << setw(5) << setfill('0') << internal << accumulator << "\n"
		<< "instructionCounter    \t    " << setw(2) << setfill('0') << noshowpos << internal << instructionCounter << "\n"
		<< "instructionRegister    \t " << setw(5) << setfill('0') << showpos << internal << instructionRegister << "\n"
		<< "operationCode    \t    " << setw(2) << setfill('0') << noshowpos << operationCode << "\n"
		<< "operand        \t\t    " << setw(2) << setfill('0') << noshowpos << internal << operand << "\n\n"
		<< "ПАМЯТЬ:" << endl;

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