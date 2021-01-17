#pragma once
#include <utility>
#include "../ConsolEngine/BaseApp.h"
#include "Figure.h"
#include "Field.h"

class Tetaeon : public BaseApp
{
private:
	static const short mBorderSize;
	static const COORD mFieldSize;
	static const short mInfoWidth;
    static const short mInfoFigureHeight;
	static const COORD mInfoFigureCenterPos;
	static const COORD mInfoScorePos;
    static const COORD mInfoCreditsDev;
    static const COORD mInfoCreditsFor;

    static const Figure mFigures[];
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
	void SetInfo(std::wstring str, COORD pos);
    void SetInfoVertical(std::wstring str, COORD pos);
	std::function<bool(COORD const&)> GetFieldConsistentChecker();
    Figure GetRandFigure() const;


    void UpdateFigureF(float dtsec);
    void SetScore();
    void PrintField();
    void PrintCurFigure();
    void ClearInfoBox();
    void PrintNextFigure();
    std::pair<wchar_t, int> ToSymbolAttributes(CellType cellType);

public:
	Tetaeon();
	virtual void KeyPressed(int btnCode);
	virtual void UpdateF(float dtsec);
};
