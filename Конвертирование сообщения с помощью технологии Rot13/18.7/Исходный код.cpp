#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

void rot13(string &);
void decodeRot13(string &);

int main()
{
	string mess("This message will be converted using technology Rot13");

	cout << "String before encryption:\n" << mess << endl;

	rot13(mess);

	cout << "\nString after encryption:\n" << mess << endl;

	decodeRot13(mess);

	cout << "\nSource string after decode:\n" << mess << endl;

	return 0;
}

void rot13(string &convertString)
{
	string alphabet("AaBbCcDdEeFfGgHhIiJjKkLlMm"
					"NnOoPpQqRrSsTtUuVvWwXxYyZz");

	for (int i = 0; i < convertString.length(); i++)
	{
		int pos = alphabet.find(convertString[i]);

		if (pos != -1)
			convertString[i] = (pos > 26 ? alphabet[pos - 13*2] : alphabet[pos + 13*2]);
	}
}

void decodeRot13(string &decodeString)
{
	string alphabet("AaBbCcDdEeFfGgHhIiJjKkLlMm"
					"NnOoPpQqRrSsTtUuVvWwXxYyZz");

	for (int i = 0; i < decodeString.length(); i++)
	{
		int pos = alphabet.find(decodeString[i]);

		if (pos != -1)
			decodeString[i] = (pos > 26 ? alphabet[pos - 13 * 2] : alphabet[pos + 13 * 2]);
	}
}