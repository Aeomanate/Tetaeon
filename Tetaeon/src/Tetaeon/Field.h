#pragma once
#include <vector>
#include <Windows.h>
#include "CellTypes.h"

class Field
{ // This class store fixed cells from figures that had falled 
private:
    const int SIZE_X, SIZE_Y, CENTER_X;
    std::vector<std::shared_ptr<CellProperties>> mCellsProperties;

    void SetCellProperties(DisplayCell&& displayCell);
    std::shared_ptr<CellProperties>& GetCellProperties(COORD coord);

    bool IsRowRegular(short y);
    void ShiftDownFieldAt(COORD coord);
    void ShiftDownFieldOnRow(short y);

public:
    Field(int sizeX, int sizeY);

    std::shared_ptr<CellProperties const> GetCellProperties(COORD coord) const;

    // Insert taken cells into field 
    bool StealCells(std::vector<DisplayCell>&& cells);

    int UpdateCells(float dtSec);
    void UpdateFilledRows();
    int Update(float dtSec);
};

