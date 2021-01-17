#pragma once
#include <Windows.h>

enum class CellType {
    free,
    regular,
    circleExplosion,
    triangleExplosionDown,
    triangleExplosionUp
};

struct Cell {
    COORD coord;
    CellType type = CellType::regular;
};