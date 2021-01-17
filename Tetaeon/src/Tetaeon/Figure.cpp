#include "Figure.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "../Debug.h"

bool Figure::State::operator==(State const& other) const 
{
	return
		rotation == other.rotation and
		pos.X == other.pos.X and
		pos.Y == other.pos.Y;
}

std::function<bool(COORD const&)> Figure::mFieldConsistentChecker;

Figure::Figure(std::vector<Cell> points)
: original(points)
, cached({ original, {0, 0}, 0 })
, cur(cached)
{ }

bool Figure::Recalc(bool isCheckConsistent)
{
	double angle = M_PI_2 * cur.rotation;

	for (size_t i = 0; i != original.size(); ++i) 
    {
        cur.cells[i] = original[i];
        cur.cells[i] = original[i];

        for (int rotates = 0; rotates != cur.rotation; ++rotates)
        {
            short prevX = cur.cells[i].coord.X;
            short prevY = cur.cells[i].coord.Y;
            cur.cells[i].coord.X = -prevY;
            cur.cells[i].coord.Y = prevX;
        }
		cur.cells[i].coord.X += cur.pos.X;
		cur.cells[i].coord.Y += cur.pos.Y;
	}
    return CacheOrDischargeTransform(isCheckConsistent);
}

bool Figure::CacheOrDischargeTransform(bool isCheckConsistent)
{
    if (isCheckConsistent)
    {
        // Dout() << "   - Check -   " << std::endl;
        for (auto const& point : cur.cells)
        {
            if (!mFieldConsistentChecker(point.coord))
            {
                cur = cached;
                return false;
            }
        }
    }
    cached = cur;
    return true;
}

bool Figure::Move(COORD offset, bool isCheckConsistent)
{
	cur.pos.X += offset.X;
	cur.pos.Y += offset.Y;
	return Recalc(isCheckConsistent);
}

bool Figure::Rotate()
{
	cur.rotation = (cur.rotation + 1) % 4;
	return Recalc();
}

void Figure::SetFieldConsistentChecker(std::function<bool(COORD const&)> newFieldConsistentChecker)
{
    mFieldConsistentChecker = newFieldConsistentChecker;
}

std::vector<Cell> const& Figure::GetCells()
{
	return cached.cells;
}
