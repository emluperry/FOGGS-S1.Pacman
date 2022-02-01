#include "Header.h"

int main()
{
    int w = 0;
    int h = 0;
    InputRectangle(w, h);

    OutputRectangleArea(w, h);
    OutputRectanglePerimeter(w, h);
    return 0;
}

void InputRectangle(int& width, int& height)
{
    cout << "Please input the width of the rectangle in cm." << endl;
    cin >> width;
    cout << "Please input the height of the rectangle in cm." << endl;
    cin >> height;
}

void OutputRectangleArea(const int width, const int height)
{
    cout << "The area is " << width * height << " centimetres squared." << endl;
}

void OutputRectanglePerimeter(const int width, const int height)
{
    cout << "The perimeter is " << width * 2 + height * 2 << " centimetres." << endl;
}