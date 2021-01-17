#include "Tetaeon.h"
#include "../Debug.h"

const short Tetaeon::mBorderSize = 1;
const COORD Tetaeon::mFieldSize = { 15, 20 };
const short Tetaeon::mInfoWidth = 7;
const short Tetaeon::mInfoFigureHeight = 4;
const COORD Tetaeon::mInfoFigureCenterPos = {
    mFieldSize.X - mInfoWidth/2 + mInfoWidth%2, 
    mInfoFigureHeight-1 
};
const COORD Tetaeon::mInfoScorePos = { 
	mBorderSize + mFieldSize.X + mBorderSize,
    mBorderSize + mInfoFigureHeight + mBorderSize
};
const COORD Tetaeon::mInfoCreditsDev = {
    mInfoScorePos.X,
    mInfoScorePos.Y + mBorderSize + 1
};
const COORD Tetaeon::mInfoCreditsFor = {
    mInfoScorePos.X + mInfoWidth - 3,
    mInfoScorePos.Y + mBorderSize + 1
};

// X to right, Y to down
// Each coord is upper left corner of point
const Figure Tetaeon::mFigures[] =
{
    // Template
    // {{
    //     {-1, 1}, {0, 1}, {1,  1},
    //     {-1, 0}, {0, 0}, {1,  0},
    //     {-1, 1}, {0, 1}, {1,  1}
    // }},

    // I
    {{
        {0, -2},
        {0, -1},
        {0,  0},
        {0,  1}
    }},

    // J
    {{
                 {0, -1},
                 {0,  0},
        {-1, 1}, {0,  1},

    }},

    // L
    {{
        {0, -1},
        {0,  0},
        {0,  1}, {1, 1}
    }},


    // O
    {{
        {-1, 1}, {0, 1},
        {-1, 0}, {0, 0}
    }},

    // S
    {{
                  {0,  0}, {0, -1},
        {-1,  1}, {0,  1},
    }},


    // Z
    {{
        {-1,  0}, {0,  0},
                  {0,  1}, {1, 1}
    }},


    // T
    {{
        {-1, 0}, {0, 0}, {1, 0},
                 {0, 1}
    }},

    // // U
    // {{
    //     {-1, 0},         {1, 0},
    //     {-1, 1}, {0, 1}, {1, 1}
    // }},
    // 
    // 
    // // Y
    // {{
    //     {-1, -1},         {1, -1},
    //     {-1,  0}, {0, 0}, {1,  0},
    //               {0, 1}
    // }},
    // 
    // // +
    // {{
    //              {0, 1},
    //     {-1, 0}, {0, 0}, {1, 0},
    //              {0, 1}
    // }},

};

Tetaeon::Tetaeon()
: BaseApp(mBorderSize + mFieldSize.X + mInfoWidth + mBorderSize, mBorderSize + mFieldSize.Y)
, mScore(0)
, mField(mFieldSize.X, mFieldSize.Y)
, mCurFigure(GetRandFigure())
, mNextFigure(GetRandFigure())
, mFallIntervalSec(0.3f)
, mSinceLastFallSec(0.0f)
, mGameMode(GameMode::playing)
{
	int top = 0;
	int left = 0;
	int right = mBorderSize + mFieldSize.X;
	int bottom = mBorderSize + mFieldSize.Y;

	int infoRight = right + mInfoWidth + mBorderSize;
	int infoX1 = right + 1;
	int infoX2 = right + mInfoWidth;

	int infoFigureEnd = mBorderSize + mInfoFigureHeight;
	int infoScoreEnd = mInfoScorePos.Y + 1;

	for (int i = left+1; i != right; ++i) {
		SetChar(i, top   , L'═');
		SetChar(i, bottom, L'═');
	}

	for (int j = top+1; j != bottom; ++j) {
		SetChar(left     , j, L'║');
		SetChar(right    , j, L'║');
		SetChar(infoRight, j, L'║');
	}

	for (int i = infoX1; i <= infoX2; ++i) {
		SetChar(i, top          , L'═');
		SetChar(i, bottom       , L'═');
		SetChar(i, infoFigureEnd, L'═');
		SetChar(i, infoScoreEnd , L'═');
	}

	SetChar(left     , top   , L'╔');
	SetChar(left     , bottom, L'╚');
	SetChar(right    , top   , L'╦');
	SetChar(right    , bottom, L'╩');
	SetChar(infoRight, top   , L'╗');
	SetChar(infoRight, bottom, L'╝');

	SetChar(right    , infoFigureEnd, L'╠');
	SetChar(infoRight, infoFigureEnd, L'╣');

	SetChar(right, infoScoreEnd    , L'╠');
	SetChar(infoRight, infoScoreEnd, L'╣');

    SetInfo(L"dev", mInfoCreditsDev);
    SetInfoVertical(L"Aeomanate", { mInfoCreditsDev.X + 1, mInfoCreditsDev.Y + 3 });
    SetInfo(L"for", mInfoCreditsFor);
    SetInfoVertical(L"Nordcurrent", { mInfoCreditsFor.X + 1, mInfoCreditsFor.Y + 2 });

    Figure::SetFieldConsistentChecker(GetFieldConsistentChecker());
}

void Tetaeon::SetInfo(std::wstring str, COORD pos)
{
	for (auto i = str.begin(); i != str.end(); ++i) {
		SetChar(pos.X + std::distance(str.begin(), i), pos.Y, *i);
	}
}

void Tetaeon::SetInfoVertical(std::wstring str, COORD pos)
{
    for (auto i = str.begin(); i != str.end(); ++i) {
        SetChar(pos.X, pos.Y + std::distance(str.begin(), i), *i);
    }
}

std::function<bool(COORD const&)> Tetaeon::GetFieldConsistentChecker() 
{
	return [this](COORD const& c) {
        if (c.Y <= 0) return true;

        bool isXConsistent = mBorderSize <= c.X and c.X < mBorderSize + mFieldSize.X;
		bool isYConsistent = c.Y < mBorderSize + mFieldSize.Y;
        bool isConsistent = isXConsistent and isYConsistent and mField.GetCell(c) == CellType::free;

        // Dout() << "{ " <<isXConsistent << isYConsistent << isConsistent << ". ";
        // Dout() << "(" << std::setw(2) << c.X << ", " << std::setw(2) << c.Y << ") } " << std::endl;

		return isConsistent;
	};
}

Figure Tetaeon::GetRandFigure() const 
{
    Figure new_figure = mFigures[rand() % std::size(mFigures)];
    new_figure.Move({ mFieldSize.X / 2, 0 }, false); // Center and out of top of field
    return new_figure;
}

void Tetaeon::KeyPressed(int btnCode) 
{
    switch (btnCode) {
        case 75: mCurFigure.Move({ -1, 0 }); break; // Left 
        case 77: mCurFigure.Move({  1, 0 }); break; // Right
        case 80: mCurFigure.Move({  0, 1 }); break; // Down 
        case 32: mCurFigure.Rotate(); break;
        default : break;
    }
}

void Tetaeon::UpdateFigureF(float dtsec) 
{
    mSinceLastFallSec += dtsec;

    if (mSinceLastFallSec < mFallIntervalSec) return;
    mSinceLastFallSec = 0;

    bool isMoved = mCurFigure.Move({ 0, 1 });
    if (!isMoved)
    {
        // Dout() << "   *** Place figure ***   " << std::endl << std::endl;
        // for (short y = 1; y <= mFieldSize.Y; ++y) {
        //     Dout() << std::setw(3) << std::left << y;
        //     for (short x = 1; x <= mFieldSize.X; ++x) {
        //         Dout() << (int) mField.GetCell({ x, y }) << " ";
        //     }
        //     Dout() << std::endl;
        // }

        if (mField.addCells(mCurFigure.GetCells()))
        {
            mScore += mField.RemoveHorizontals();
            mCurFigure = mNextFigure;
            mNextFigure = GetRandFigure();
        }
        else
        {
            throw std::exception("End game");
        }
    }
}

std::pair<wchar_t, int> Tetaeon::ToSymbolAttributes(CellType cellType) {
    wchar_t symbol;
    int attributes = 0;
    switch (cellType) {
        case CellType::regular:
            symbol = L'█';
            attributes = FOREGROUND_GREEN;
            break;

        case CellType::circleExplosion:
            symbol = L'A';
            attributes = FOREGROUND_GREEN;
            break;

        case CellType::triangleExplosionDown:
            symbol = L'B';
            attributes = FOREGROUND_GREEN;
            break;

        case CellType::triangleExplosionUp:
            symbol = L'C';
            attributes = FOREGROUND_GREEN;
            break;

        case CellType::free:
            symbol = L'∙';
            attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            break;

        default:
            throw std::logic_error("Undefined cell type");
            break;
    }

    return std::make_pair(symbol, attributes);
}

void Tetaeon::UpdateF(float dtsec)
{
    UpdateFigureF(dtsec);
    SetScore();
    PrintField();
    PrintCurFigure();
    ClearInfoBox();
    PrintNextFigure();

}

void Tetaeon::SetScore()
{
    std::wstring str_score = std::to_wstring(mScore);
    int free_space = mInfoWidth - str_score.size();
    str_score = std::wstring(free_space / 2, L' ') + str_score;
    SetInfo(str_score, mInfoScorePos);
}

void Tetaeon::PrintField() 
{
    for (short y = 1; y <= mFieldSize.Y; ++y) {
        for (short x = 1; x <= mFieldSize.X; ++x) {
            auto sa = ToSymbolAttributes(mField.GetCell({ x, y }));
            SetChar(x, y, sa.first, sa.second);
        }
    }
}

void Tetaeon::PrintCurFigure() 
{
    for (Cell const& cell : mCurFigure.GetCells())
    {
        if (cell.coord.Y >= 1)
        {
            auto sa = ToSymbolAttributes(cell.type);
            SetChar(cell.coord.X, cell.coord.Y, sa.first, sa.second);
        }
    }
}

void Tetaeon::ClearInfoBox()
{
    for (short y = mBorderSize; y != mBorderSize + mInfoFigureHeight; ++y)
    {
        for (short x = mBorderSize + mFieldSize.X + mBorderSize; x != 2 * mBorderSize + mFieldSize.X + mInfoWidth; ++x)
        {
            auto sa = ToSymbolAttributes(CellType::free);
            SetChar(x, y, sa.first, sa.second);
        }
    }
}

void Tetaeon::PrintNextFigure()
{
    mNextFigure.Move(mInfoFigureCenterPos, false);
    for (Cell const& cell : mNextFigure.GetCells())
    {
        auto sa = ToSymbolAttributes(cell.type);
        SetChar(cell.coord.X, cell.coord.Y, sa.first, sa.second);
    }
    mNextFigure.Move({ -mInfoFigureCenterPos.X, -mInfoFigureCenterPos.Y });
}

