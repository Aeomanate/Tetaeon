#include "Tetaeon.h"

const short Tetaeon::BORDER_SIZE = 1;
const COORD Tetaeon::FIELD_SIZE = { 15, 20 };
const short Tetaeon::INFO_WIDTH = 7;
const short Tetaeon::INFO_FIGURE_HEIGHT = 4;
const COORD Tetaeon::INFO_FIGURE_CENTER_POS = {
    FIELD_SIZE.X - INFO_WIDTH/2 + INFO_WIDTH%2, 
    INFO_FIGURE_HEIGHT-1 
};
const COORD Tetaeon::INFO_SCORE_POS = { 
	BORDER_SIZE + FIELD_SIZE.X + BORDER_SIZE,
    BORDER_SIZE + INFO_FIGURE_HEIGHT + BORDER_SIZE
};
const COORD Tetaeon::INFO_CREDITS_DEV_POS = {
    INFO_SCORE_POS.X,
    INFO_SCORE_POS.Y + BORDER_SIZE + 1
};

// X to right, Y to down
const Figure Tetaeon::FIGURES[] =
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
                  {0,  0}, {1, 0},
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
: BaseApp(BORDER_SIZE + FIELD_SIZE.X + INFO_WIDTH + BORDER_SIZE, BORDER_SIZE + FIELD_SIZE.Y)
, mScore(0)
, mField(FIELD_SIZE.X, FIELD_SIZE.Y)
, mCurFigure(GetRandFigure())
, mNextFigure(GetRandFigure())
, mFallIntervalSec(0.3f)
, mSinceLastFallSec(0.0f)
, mGameMode(GameMode::playing)
{
	short top = 0;
	short left = 0;
	short right = BORDER_SIZE + FIELD_SIZE.X;
	short bottom = BORDER_SIZE + FIELD_SIZE.Y;
    
	short infoRight = right + INFO_WIDTH + BORDER_SIZE;
	short infoX1 = right + 1;
	short infoX2 = right + INFO_WIDTH;
    
	short infoFigureEnd = BORDER_SIZE + INFO_FIGURE_HEIGHT;
	short infoScoreEnd = INFO_SCORE_POS.Y + 1;

	for (short i = left+1; i != right; ++i) {
		SetChar({i, top   }, L'═');
		SetChar({i, bottom}, L'═');
	}

	for (short j = top+1; j != bottom; ++j) {
		SetChar({left     , j}, L'║');
		SetChar({right    , j}, L'║');
		SetChar({infoRight, j}, L'║');
	}

	for (short i = infoX1; i <= infoX2; ++i) {
		SetChar({i, top          }, L'═');
		SetChar({i, bottom       }, L'═');
		SetChar({i, infoFigureEnd}, L'═');
		SetChar({i, infoScoreEnd }, L'═');
	}

	SetChar({left     , top   }, L'╔');
	SetChar({left     , bottom}, L'╚');
	SetChar({right    , top   }, L'╦');
	SetChar({right    , bottom}, L'╩');
	SetChar({infoRight, top   }, L'╗');
	SetChar({infoRight, bottom}, L'╝');
            
	SetChar({right    , infoFigureEnd}, L'╠');
	SetChar({infoRight, infoFigureEnd}, L'╣');
            
	SetChar({right    , infoScoreEnd}, L'╠');
	SetChar({infoRight, infoScoreEnd}, L'╣');

    SetInfoHorizontal(L"Credits", INFO_CREDITS_DEV_POS);
    SetInfoVertical(L"Aeomanate", { INFO_CREDITS_DEV_POS.X + INFO_WIDTH / 2, INFO_CREDITS_DEV_POS.Y + 2 });

    Figure::SetFieldConsistentChecker(CreateFieldConsistentChecker()); 
    PrintNextFigure();
}

void Tetaeon::SetInfoHorizontal(std::wstring str, COORD pos)
{
	for (auto i = str.begin(); i != str.end(); ++i) {
        SetChar(pos + COORD { static_cast<short>(std::distance(str.begin(), i)), 0}, *i);
	}
}

void Tetaeon::SetInfoVertical(std::wstring str, COORD pos)
{
    for (auto i = str.begin(); i != str.end(); ++i) {
        SetChar(pos + COORD{ 0, static_cast<short>(std::distance(str.begin(), i))}, *i);
    }
}

std::function<bool(COORD const&)> Tetaeon::CreateFieldConsistentChecker()
{
	return [this](COORD const& c) {
        if (c.Y <= 0) return true;

        bool isXConsistent = BORDER_SIZE <= c.X and c.X < BORDER_SIZE + FIELD_SIZE.X;
		bool isYConsistent = c.Y < BORDER_SIZE + FIELD_SIZE.Y;
        bool isFieldConsistent = isXConsistent and isYConsistent and mField.GetCellProperties(c)->IsFree();

		return isFieldConsistent;
	};
}

Figure Tetaeon::GetRandFigure() const 
{
    Figure new_figure(FIGURES[rand() % std::size(FIGURES)]);
    new_figure.Move({ FIELD_SIZE.X / 2, 0 }, Figure::IsConsistentCheck::DONT_CHECK); // Center and out of top of field
    new_figure.MutateCells();

    new_figure.setUpdateDelay(0.55 - 0.044*log(90+mScore));

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

void Tetaeon::UpdateFigureF(float dtSec) 
{
    if (mCurFigure.Update(dtSec)) return;
    if(!mField.SetCells(mCurFigure.GrabCells())) 
        throw std::runtime_error("End game");

    mCurFigure = mNextFigure;
    mNextFigure = GetRandFigure();

    PrintNextFigure();
}

void Tetaeon::UpdateF(float dtSec)
{
    SetScore();
    UpdateFigureF(dtSec);
    mScore += mField.Update(dtSec);

    PrintField(dtSec);
    PrintCurFigure();
}

void Tetaeon::SetScore()
{
    std::wstring str_score = std::to_wstring(mScore);
    int free_space = INFO_WIDTH - str_score.size();
    str_score = std::wstring(free_space / 2 + free_space % 2, L' ') + str_score;
    SetInfoHorizontal(str_score, INFO_SCORE_POS);
}

void Tetaeon::PrintField(float dtSec)
{
    for (short y = 1; y <= FIELD_SIZE.Y; ++y) {
        for (short x = 1; x <= FIELD_SIZE.X; ++x) {
            SetChar({ x, y }, mField.GetCellProperties({ x, y })->GetCellStyle());
        }
    }
}

void Tetaeon::PrintCurFigure() 
{
    for (DisplayCell const& cell : mCurFigure.GrabCells())
    {
        if (cell.mPosOnField.Y >= 1)
        {
            SetChar(cell.mPosOnField, cell.mProperties->GetCellStyle());
        }
    }
}

void Tetaeon::ClearInfoBox()
{
    for (short y = BORDER_SIZE; y != BORDER_SIZE + INFO_FIGURE_HEIGHT; ++y)
    {
        for (short x = BORDER_SIZE + FIELD_SIZE.X + BORDER_SIZE; x != 2 * BORDER_SIZE + FIELD_SIZE.X + INFO_WIDTH; ++x)
        {
            SetChar({ x, y }, FreeCell::GetCellStyleStatic());
        }
    }
}

void Tetaeon::PrintNextFigure()
{
    ClearInfoBox();
    mNextFigure.Move(INFO_FIGURE_CENTER_POS, Figure::IsConsistentCheck::DONT_CHECK);
    for (DisplayCell const& cell : mNextFigure.GrabCells())
    {
        SetChar(cell.mPosOnField, cell.mProperties->GetCellStyle());
    }
    mNextFigure.Move(-INFO_FIGURE_CENTER_POS, Figure::IsConsistentCheck::DONT_CHECK);
}

