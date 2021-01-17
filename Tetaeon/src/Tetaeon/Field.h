#pragma once
#include <vector>
#include <Windows.h>
#include "CellTypes.h"

class Field
{ // This class store fixed cells from figures that had falled 
private:
    int xSize, ySize;
    std::vector<CellType> cells;

public:
    Field(int xSize, int ySize);

    void SetCell(COORD coord, CellType cellType);

    CellType GetCell(COORD coord);

    bool addCells(std::vector<Cell> const& cells);

    int RemoveHorizontals();
};

