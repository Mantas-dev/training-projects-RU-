#include <iostream>

#include "Complex.h"

Complex::Complex(double realPart, double imaginaryPart)
:real(realPart), imaginary(imaginaryPart)
{

}

Complex Complex::operator + (const Complex &operand2) const
{
	return Complex(real + operand2.real, imaginary + operand2.imaginary);
}

Complex Complex::operator - (const Complex &operand2) const
{
	return Complex(real - operand2.real, imaginary - operand2.imaginary);
}

Complex Complex::operator* (const Complex &operand2) const
{
	return Complex((real * operand2.real) - (imaginary * operand2.imaginary), (real * operand2.imaginary) + (operand2.real * imaginary));
}

const Complex &Complex::operator= (const Complex &right)
{
	if (&right != this)
	{
		real = right.real;
		imaginary = right.imaginary;
	}

	return *this;
}

bool Complex::operator== (const Complex &right) const
{
	if ((real != right.real) || (imaginary != right.imaginary))
		return false;

	return true;

}

ostream & operator<< (ostream &output, const Complex &c)
{
	output << '(' << c.real << ", " << c.imaginary << ')';
	
	return output;
}

istream & operator>> (istream &input, Complex &c)
{
	input >> c.real >> c.imaginary;

	return input;
}