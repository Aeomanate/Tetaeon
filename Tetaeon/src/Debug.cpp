#include "Debug.h"

std::ofstream& Dout() {
    static std::ofstream debug_output("debug_output.txt", std::ios_base::app);
    return debug_output;
}