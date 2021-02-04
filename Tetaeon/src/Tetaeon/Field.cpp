#include "Field.h"

Field::Field(int sizeX, int sizeY)
: SIZE_X(sizeX)
, SIZE_Y(sizeY)
, CENTER_X(SIZE_X / 2 + SIZE_X % 2)
, mCellsProperties(sizeX * sizeY)
{ 
    for (std::shared_ptr<CellProperties>& cellProperties : mCellsProperties)
    {
        cellProperties.reset(new FreeCell());
    }
}

std::shared_ptr<CellProperties>& Field::GetCellProperties(COORD coord)
{
    coord -= {1, 1};
    return mCellsProperties[coord.Y * SIZE_X + coord.X];
}

std::shared_ptr<CellProperties const> Field::GetCellProperties(COORD coord) const
{
    return const_cast<Field*>(this)->GetCellProperties(coord);
}

void Field::SetCellProperties(DisplayCell&& displayCell)
{
    displayCell.mPosOnField -= {1, 1};
    mCellsProperties[displayCell.mPosOnField.Y * SIZE_X + displayCell.mPosOnField.X] = displayCell.mProperties;
}

bool Field::StealCells(std::vector<DisplayCell>&& displayCells)
{
    for (DisplayCell& displayCell : displayCells)
    {
        // Game is end when we try to place out of field figure 
        if (displayCell.mPosOnField.Y <= 0) return false;
        SetCellProperties(std::move(displayCell));
    }
    return true;
}

bool Field::IsRowRegular(short y) 
{
    for (short x = 1; x <= SIZE_X; ++x)
    {
        std::shared_ptr<CellProperties>& curCell = GetCellProperties({ x, y });
        if (curCell->IsFree() or curCell->IsActivated() or curCell->isGravityAbove())
        {
            return false;
        }
    }
    return true;
}

void Field::ShiftDownFieldAt(COORD coord)
{
    // Insert cells above into cells belowe one times
    for (short curY = coord.Y; curY > 1; --curY)
    {
        SetCellProperties(DisplayCell{ {coord.X, curY}, GetCellProperties({coord.X, curY-1}) });
    }

    COORD topOfField = { coord.X, 1 };
    std::shared_ptr<CellProperties>& topCell = GetCellProperties(topOfField);
    if (!topCell->IsFree())
    {
        SetCellProperties(DisplayCell{ topOfField, std::make_shared<FreeCell>() });
    }
}

void Field::ShiftDownFieldOnRow(short y)
{
    for (short x = 1; x <= SIZE_X; ++x)
    {
        ShiftDownFieldAt({ x, y });
    }
}

int Field::UpdateCells(float dtSec)
{
    int score = 0;

    for (short y = 1; y <= SIZE_Y; ++y)
    {
        for (short x = 1; x <= SIZE_X; ++x)
        {
            std::shared_ptr<CellProperties>& curCell = GetCellProperties({ x, y });

            if (curCell->IsReadyToDestroy())
            {
                ShiftDownFieldAt({ x, y });
                score += 1;
            }

            StealCells(curCell->Update({ x, y }, dtSec, *this));
        }
    }

    return score;

}

void Field::UpdateFilledRows()
{
    for (short y = 1; y <= SIZE_Y; ++y)
    {
        if (IsRowRegular(y))
        {

            int updatesToDestroy = CENTER_X;
            short xToRight = 1;
            short xToLeft = SIZE_X;

            for(int i = 0; i != CENTER_X; ++i)
            {
                if (!GetCellProperties({xToRight, y})->TryActivate(i))
                {
                    SetCellProperties(DisplayCell{ {xToRight, y}, std::make_shared<HorizontalLineCell>(i, updatesToDestroy) });
                }
                if (!GetCellProperties({xToLeft, y})->TryActivate(i))
                {
                    SetCellProperties(DisplayCell{ {xToLeft, y}, std::make_shared<HorizontalLineCell>(i, updatesToDestroy) });
                }
                xToRight += 1;
                xToLeft -= 1;
            }
        }
    }
}

int Field::Update(float dtSec)
{
    int score = UpdateCells(dtSec);
    UpdateFilledRows();
    return score;
}