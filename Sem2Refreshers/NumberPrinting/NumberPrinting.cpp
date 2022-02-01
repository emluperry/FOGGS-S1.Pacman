#include "Header.h"

int main()
{
    int num = InputNumber();
    GeneratePrint(num);
}

int InputNumber()
{
    cout << "Input a number between 1 and 10 to print. ";
    int num = 0;
    do
    {
        cin >> num;
    } while (!(num >= 1 && num <= 10));
    cout << endl;
    return num;
}

void GeneratePrint(int num)
{
    for (int i = 0; i < num; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            cout << j+1 << " ";
        }
        cout << endl;
    }
}