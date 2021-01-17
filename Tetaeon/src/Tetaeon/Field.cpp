#include "Field.h"

Field::Field(int xSize, int ySize)
: xSize(xSize)
, ySize(ySize)
, cells(xSize * ySize, CellType::free)
{

}

void Field::SetCell(COORD coord, CellType cellType)
{
    cells[(coord.Y - 1) * xSize + (coord.X - 1)] = cellType;
}

CellType Field::GetCell(COORD coord) 
{
    return cells[(coord.Y - 1) * xSize + (coord.X - 1)];
}

bool Field::addCells(std::vector<Cell> const & cells)
{
    for (Cell const& cell : cells)
    {
        if (cell.coord.Y <= 1) return false;
        SetCell(cell.coord, cell.type);
    }
    return true;
}

int Field::RemoveHorizontals()
{
    int score = 0;

    // Find filled rows
    short y = ySize;
    while (y >= 1)
    {
        bool isRowFilled = true;
        for (short x = 1; x <= xSize; ++x)
        {
            if (GetCell({ x, y }) == CellType::free)
            {
                isRowFilled = false;
                break;
            }
        }
        if (!isRowFilled)
        {
            --y;
            continue;
        }

        // Shift down game field to current filled row
        for (short yAbove = y - 1; yAbove >= 1; --yAbove)
        {
            for (short x = 1; x <= xSize; ++x)
            {
                SetCell({ x, yAbove + 1 }, GetCell({ x, yAbove }));
            }
        }

        score += xSize;
    }

    return score;
}
