#include "Header.h"

int main()
{
	int numbers[10];
	for (int i = 0; i < 10; i++)
	{
		cout << "Please input an integer." << endl;
		cin >> numbers[i];
	}

	OutputHighest(numbers);
}

void OutputHighest(int values[10])
{
	int highest = values[0];
	for (int i = 1; i < 10; i++)
	{
		if (highest < values[i])
		{
			highest = values[i];
		}
	}
	cout << "The highest number is " << highest;
}