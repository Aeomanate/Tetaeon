#include "Figure.h"
#define _USE_MATH_DEFINES
#include <math.h>

bool Figure::State::operator==(State const& other) const 
{
	return
		rotation == other.rotation and
		mPosOnField.X == other.mPosOnField.X and
		mPosOnField.Y == other.mPosOnField.Y;
}

int Figure::FIGURE_MUTATE_PROBABILITY = 20;
int Figure::CELL_MUTATE_PROBABILITY = 20;

std::function<bool(COORD const&)> Figure::mFieldConsistentChecker;

Figure::Figure(std::vector<DisplayCell> displayCells)
: mOriginalDisplayCells(displayCells)
{ }

Figure::Figure(Figure const& other)
: mIntervalFall(other.mIntervalFall)
, mSinceLastFallSec(0.0f)
{
    // Be cause the properties is shared_ptr
    // we need deep copy for set new figure via copy of preset figure
    mOriginalDisplayCells.reserve(other.mOriginalDisplayCells.size());
    for (int i = 0; i != other.mOriginalDisplayCells.size(); ++i) 
    {
        mOriginalDisplayCells.push_back(
            DisplayCell { 
                other.mOriginalDisplayCells[i].mPosOnField, 
                other.mOriginalDisplayCells[i].mProperties->clone()
            }
        );
    }
    mTempState = other.mTempState;
    mCached = other.mCached;

}

void Figure::setUpdateDelay(float new_delay)
{
    mIntervalFall = new_delay;
}

bool Figure::Recalc(IsConsistentCheck isConsistentCheck)
{
    mTempState.mDisplayCells = mOriginalDisplayCells;
	for (size_t i = 0; i != mOriginalDisplayCells.size(); ++i) 
    {
        for (int rotates = 0; rotates != mTempState.rotation; ++rotates)
        {
            short prevX = mTempState.mDisplayCells[i].mPosOnField.X;
            short prevY = mTempState.mDisplayCells[i].mPosOnField.Y;
            mTempState.mDisplayCells[i].mPosOnField.X = -prevY;
            mTempState.mDisplayCells[i].mPosOnField.Y = prevX;
        }
		mTempState.mDisplayCells[i].mPosOnField += mTempState.mPosOnField;
	}
    return TryCacheTransform(isConsistentCheck);
}

bool Figure::TryCacheTransform(IsConsistentCheck isConsistentCheck)
{
    if (isConsistentCheck == IsConsistentCheck::DO_CHECK)
    {
        for (DisplayCell const& displayCell : mTempState.mDisplayCells)
        {
            if (!mFieldConsistentChecker(displayCell.mPosOnField))
            {
                mTempState = mCached;
                return false;
            }
        }
    }
    mCached = mTempState;
    return true;
}

bool Figure::Move(COORD offset, IsConsistentCheck isConsistentCheck)
{
	mTempState.mPosOnField += offset;
	return Recalc(isConsistentCheck);
}

bool Figure::Rotate()
{
	mTempState.rotation = (mTempState.rotation + 1) % 4;
	return Recalc();
}

void Figure::MutateCells()
{
    if (rand() % 101 > FIGURE_MUTATE_PROBABILITY) return;

    for (int i = 0; i != mOriginalDisplayCells.size(); ++i) {
        if (rand() % 101 > CELL_MUTATE_PROBABILITY) continue;
        mTempState.mDisplayCells[i].mProperties = 
        mCached.mDisplayCells[i].mProperties = 
        mOriginalDisplayCells[i].mProperties = std::make_shared<CircleExplosionCell>();
        return;
    }
}

bool Figure::Update(float dtSec)
{
    mSinceLastFallSec += dtSec;
    if (mSinceLastFallSec < mIntervalFall) return true;
    mSinceLastFallSec = 0;

    return Move({ 0, 1 });
}

void Figure::SetFieldConsistentChecker(std::function<bool(COORD const&)> newFieldConsistentChecker)
{
    mFieldConsistentChecker = newFieldConsistentChecker;
}

std::vector<DisplayCell>&& Figure::GrabCells()
{
	return std::move(mCached.mDisplayCells);
}
