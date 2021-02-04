#pragma once
#include <Windows.h>
#include <fstream>
#include <iomanip>

std::ofstream& Dout();

struct CellStyle {
    CellStyle(wchar_t symbol, int attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    wchar_t mSymbol;
    int mAttributes;
};

COORD operator-(COORD const& a);
COORD operator+(COORD const& a, COORD const& b);
COORD operator-(COORD const& a, COORD const& b);
COORD operator+=(COORD& a, COORD const& b);
COORD operator-=(COORD& a, COORD const& b);