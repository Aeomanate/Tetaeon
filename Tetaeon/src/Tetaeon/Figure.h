#pragma once
#include <vector>
#include <functional>
#include <Windows.h>
#include "CellTypes.h"

class Figure
{
private:
	std::vector<Cell> original;  // Local position with center at (0, 0)
	struct State {
		std::vector<Cell> cells; // On game field
		COORD pos; // Position on display of figure
		int rotation; // In PI/2 chapters

		bool operator==(State const& other) const;
	} cached, cur;

    // Call for each rotate or move transform
    static std::function<bool(COORD const&)> mFieldConsistentChecker;

    // Transform original figure position and check if it permissible 
    // Return true if recalc successful and figure not restored from cache
	bool Recalc(bool isCheckConsistent = true);

    // Store to cache if consistent check is success, return true
    // Restore from cache otherwise, return false
    bool CacheOrDischargeTransform(bool isCheckConsistent);

public:
    // Call when field is constructed
    static void SetFieldConsistentChecker(std::function<bool(COORD const&)> newFieldConsistentChecker);

	// Points with center of figure at (0, 0)
	Figure(std::vector<Cell> points);

	// Move by vetor, cache coord on field
    // Return true if moved
    // Param isCheck allow disable transform checks 
	bool Move(COORD offset, bool isCheckConsistent = true);

	// Add 90 degree rotation clockwise around (0, 0) point
    // Cache coord on field
    // Return true if rotated
	bool Rotate();

	// Return transformed cached points of figure at game field
	std::vector<Cell> const& GetCells();

};

