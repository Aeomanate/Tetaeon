#pragma once
#include <vector>
#include <functional>
#include <Windows.h>
#include "CellTypes.h"


class Figure
{
public:
    enum class IsConsistentCheck { DO_CHECK, DONT_CHECK };

private:
    static int FIGURE_MUTATE_PROBABILITY;
    static int CELL_MUTATE_PROBABILITY;

	std::vector<DisplayCell> mOriginalDisplayCells;  // Local position with center at (0, 0)
	struct State {
		std::vector<DisplayCell> mDisplayCells; // On game field
		COORD mPosOnField; // Position on display of figure
		int rotation; // In PI/2 chapters

		bool operator==(State const& other) const;
	} mCached, mTempState;

    float mIntervalFall;
    float mSinceLastFallSec;

    // Call for each rotate or move transform
    static std::function<bool(COORD const&)> mFieldConsistentChecker;

    // Transform original figure position
    // Return true if new state is consistent 
	bool Recalc(IsConsistentCheck isConsistentCheck = IsConsistentCheck::DO_CHECK);

    // Return true if consistent check is success, store temp state to cache
    // Return false otherwise, restore state from cache
    bool TryCacheTransform(IsConsistentCheck isConsistentCheck);

public:
    // Call when field is constructed
    static void SetFieldConsistentChecker(std::function<bool(COORD const&)> newFieldConsistentChecker);

	// Points with center of figure at (0, 0)
	Figure(std::vector<DisplayCell> displayCells);

    // Need for deep copy of DisplayCell
    Figure(Figure const& other);

    // Set the delay of move with depend on player score
    void setUpdateDelay(float new_delay);

	// Move by vetor, cache coord on field
    // Return true if consistent check is successful 
	bool Move(COORD offset, IsConsistentCheck isConsistentCheck = IsConsistentCheck::DO_CHECK);

	// Add 90 degree rotation clockwise around (0, 0) point
    // Cache coord on field
    // Return true if rotated
	bool Rotate();

    // Random insert explosive cells 
    void MutateCells();

    // Move figure down 
    // Return true if successful moved or if not time to move
    bool Update(float dtSec);

	// Grab transformed cached points of figure at game field
	std::vector<DisplayCell>&& GrabCells();

};

