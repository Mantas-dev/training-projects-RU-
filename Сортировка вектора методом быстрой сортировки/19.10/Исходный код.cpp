#include <iostream>
using std::cout;
using std::endl;

#include <cstdlib>
using std::rand;
using std::srand;

#include <ctime>
using std::time;

#include <vector>
using std::vector;

vector<int> testVect(10);

void quickSortHelper(int, int);

int main()
{
	srand(time(0));

	cout << "Current vector:" << endl;

	for (int i = 0; i < testVect.size(); i++)
	{
		testVect[i] = 10 + rand() % 90;
		cout << testVect[i] << ' ';
	}

	cout << endl;

	quickSortHelper(0, testVect.size() - 1);

	cout << "Vector sorted by Quick Sort:" << endl;

	for (int i = 0; i < testVect.size(); i++)
	{
		cout << testVect[i] << ' ';
	}

	cout << endl;

	return 0;
}

void quickSortHelper(int left, int right)
{
	int posSortElem = left;
	int timeVar;

	int beginOfSort = left;
	int endOfSort = right;

	bool isChange = true;

	if (right - left >= 1)
	{
		while (isChange)
		{
			for (int i = endOfSort; i >= left; i--)
			{
				if (i == posSortElem){	isChange = false;	break;	}
				else
				{
					if (testVect[i] < testVect[posSortElem])
					{
						timeVar = testVect[posSortElem];
						testVect[posSortElem] = testVect[i];
						testVect[i] = timeVar;

						beginOfSort = posSortElem + 1;

						posSortElem = i;
						isChange = true;

						break;
					}
				}
			}
			
			for (int i = beginOfSort; i <= right; i++)
			{
				if (i == posSortElem){ isChange = false;	break; }
				else
				{
					if (testVect[i] > testVect[posSortElem])
					{
						timeVar = testVect[posSortElem];
						testVect[posSortElem] = testVect[i];
						testVect[i] = timeVar;

						endOfSort = posSortElem - 1;

						posSortElem = i;
						isChange = true;

						break;
					}
				}
			}			
		}
	
		if (left < posSortElem ) quickSortHelper(left, posSortElem - 1);
		if (right > posSortElem) quickSortHelper(posSortElem + 1, right);
	}
}