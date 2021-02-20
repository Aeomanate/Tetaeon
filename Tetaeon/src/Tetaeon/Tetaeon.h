#pragma once
#include <utility>
#include "../ConsolEngine/BaseApp.h"
#include "Figure.h"
#include "Field.h"

class Tetaeon : public BaseApp
{
private:
	static const short BORDER_SIZE;
	static const COORD FIELD_SIZE;
	static const short INFO_WIDTH;
    static const short INFO_FIGURE_HEIGHT;
	static const COORD INFO_FIGURE_CENTER_POS;
	static const COORD INFO_SCORE_POS;
    static const COORD INFO_CREDITS_DEV_POS;
    static const COORD INFO_CREDITS_FOR_POS;

    static const Figure FIGURES[];
	int mScore;
    Field mField;
	Figure mCurFigure;
    Figure mNextFigure;

    float mFallIntervalSec;
    float mSinceLastFallSec;

    enum GameMode {
        playing,
        award,
        stats
    } mGameMode;

private:
	void SetInfoHorizontal(std::wstring str, COORD pos);
    void SetInfoVertical(std::wstring str, COORD pos);
	std::function<bool(COORD const&)> CreateFieldConsistentChecker();
    Figure GetRandFigure() const;


    void UpdateFigureF(float dtSec);
    void SetScore();
    void PrintField(float dtSec);
    void PrintCurFigure();
    void ClearInfoBox();
    void PrintNextFigure();

public:
	Tetaeon();
	virtual void KeyPressed(int btnCode);
	virtual void UpdateF(float dtSec);
};
