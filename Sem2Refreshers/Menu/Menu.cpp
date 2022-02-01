#include "Header.h"

int main()
{
    bool inUse = true;

    while (inUse)
    {
        int weather = 0;
        weather = SelectWeather();

        switch (weather)
        {
        case 1:
            cout << "Wear sunscreen!" << endl;
            break;
        case 2:
            cout << "Wrap up warm." << endl;
            break;
        case 3:
            cout << "Bring an umbrella!" << endl;
            break;
        case 4:
            cout << "Exiting" << endl;
            inUse = false;
            break;
        default:
            cout << "Error with input." << endl;
            inUse = false;
            break;
        }
    }
}

int SelectWeather()
{
    int choice = 0;
    do
    {
        cout << "Please choose an option:\n1. Sunny\n2. Cloudy\n3. Raining\n4. Exit" << endl;
        cin >> choice;
    } while (!(choice > 0 && choice < 5));

    return choice;
}