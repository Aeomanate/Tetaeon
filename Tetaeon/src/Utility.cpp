#include "Utility.h"

std::ofstream& Dout() {
    static std::ofstream debug_output("debug_output.txt", std::ios_base::app);
    return debug_output;
}

CellStyle::CellStyle(wchar_t symbol, int attributes)
    : mSymbol(symbol)
    , mAttributes(attributes)
{ }

COORD operator-(COORD const& a) { return { -a.X, -a.Y }; }
COORD operator+(COORD const& a, COORD const& b) { return { a.X + b.X, a.Y + b.Y }; }
COORD operator-(COORD const& a, COORD const& b) { return a + -b; }
COORD operator+=(COORD& a, COORD const& b) { return a = a + b; }
COORD operator-=(COORD& a, COORD const& b) { return a = a - b; }