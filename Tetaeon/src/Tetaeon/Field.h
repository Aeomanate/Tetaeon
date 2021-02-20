#pragma once
#include <vector>
#include <Windows.h>
#include "CellTypes.h"

class Field
{ // This class store fixed cells from figures that had falled 
private:
    const int SIZE_X, SIZE_Y, CENTER_X;
    std::vector<std::shared_ptr<CellProperties>> mCellsProperties;


    bool IsRowRegular(short y);
    void ShiftDownFieldAt(COORD coord);


public:
    Field(int sizeX, int sizeY);
    int CountCols() const;
    int CountRows() const;


    std::shared_ptr<CellProperties>& GetCellProperties(COORD coord);

    // Insert taken cells into field 
    void SetCellProperties(DisplayCell&& displayCell);
    bool SetCells(std::vector<DisplayCell>&& cells);

    // Swap two cell properties by it coords 
    void SwapCells(COORD a, COORD b);

    int UpdateCells(float dtSec);
    void UpdateFilledRows();
    int Update(float dtSec);
};

