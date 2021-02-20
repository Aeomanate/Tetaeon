#include "CellTypes.h"
#include "Field.h"

void CellProperties::SetFall(bool isFall) { mIsFall = isFall; }
bool CellProperties::IsFree() const { return false; }
bool CellProperties::TryActivate(int updatesToActivate) { return false; }
bool CellProperties::IsActivated() const { return false; }
bool CellProperties::IsActivable() const { return !IsFree() and !IsActivated(); }
bool CellProperties::IsReadyToDestroy() const { return false; }

bool CellProperties::IsShiftFieldDown() const { return true; }

inline void CellProperties::Update(COORD thisCoord, float dtSec, Field & field) {
    if (mIsFall and thisCoord.Y != field.CountRows()) {
        if ((mLastFall += dtSec) < mFallInterval) return;
        mLastFall = 0;

        COORD downCoord = thisCoord + COORD{ 0, 1 };
        if (field.GetCellProperties(downCoord)->IsFree())
        {
            field.SwapCells(thisCoord, downCoord);
        }
        else {
            mIsFall = false;
        }
    }
}


bool FreeCell::IsFree() const { return true; }
CellStyle FreeCell::GetCellStyle() const
{
    return GetCellStyleStatic();
}
CellStyle FreeCell::GetCellStyleStatic()
{
    return { L'∙', FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE };
}
std::shared_ptr<CellProperties> FreeCell::clone() const
{
    return std::make_shared<FreeCell>(*static_cast<FreeCell const*>(this));
}


CellStyle FigureCell::GetCellStyle() const { return { L'█', FOREGROUND_GREEN }; }


float HorizontalLineCell::mUpdateDelaySec = 0.05f;
HorizontalLineCell::HorizontalLineCell(int updatesToShow, int updatesToDestroy)
: mStyle(FigureCell().GetCellStyle())
, mUpdatesToShown(updatesToShow)
, mUpdatesToDestroy(updatesToDestroy)
, mLastUpdateSec(0) 
{ }
CellStyle HorizontalLineCell::GetCellStyle() const { return mStyle; }
bool HorizontalLineCell::IsReadyToDestroy() const { return mUpdatesToDestroy == 0; }
bool HorizontalLineCell::TryActivate(int updatesToActivate) { return true; }
bool HorizontalLineCell::IsActivated() const { return true; }
void HorizontalLineCell::Update(COORD thisCoord, float dtSec, Field& field)
{
    CellProperties::Update(thisCoord, dtSec, field);

    if ((mLastUpdateSec += dtSec) < mUpdateDelaySec) return;
    mLastUpdateSec = 0.0f;

    if (mUpdatesToShown == 0) 
    {
        mStyle = { L'─', BACKGROUND_GREEN | FOREGROUND_RED };
    }

    mUpdatesToShown -= 1;
    mUpdatesToDestroy -= 1;
}


int CircleExplosionCell::mExplosionRadius = 2;
float CircleExplosionCell::mExplosionUpdateDelay = 0.1f;
CircleExplosionCell::CircleExplosionCell()
: mCurMarkRadius(-1)
, mLastUpdateSec(0)
, mIsActivated(false)
, mUpdatesToActivate(-1) // 0+ is need to activate (immediately or later)
, mCellStyle({ L'■', BACKGROUND_GREEN | FOREGROUND_RED | FOREGROUND_GREEN })
{ }
CellStyle CircleExplosionCell::GetCellStyle() const { return mCellStyle; }
bool CircleExplosionCell::TryActivate(int updatesToActivate)
{
    mUpdatesToActivate = updatesToActivate;
    return true;
}
bool CircleExplosionCell::IsActivated() const { return mIsActivated; }
inline bool CircleExplosionCell::IsReadyToDestroy() const { return mCurMarkRadius == mExplosionRadius+1; }
bool CircleExplosionCell::IsShiftFieldDown() const { return false; }
void CircleExplosionCell::Update(COORD thisCoord, float dtSec, Field& field)
{
    CellProperties::Update(thisCoord, dtSec, field);

    if (!mIsActivated)
    {
        if (mUpdatesToActivate < 0 or (mLastUpdateSec += dtSec) < HorizontalLineCell::mUpdateDelaySec) return;
        mLastUpdateSec = 0; 
        
        if (mUpdatesToActivate-- == 0)
        {
            mIsActivated = true;
            mCellStyle = { L'■', BACKGROUND_GREEN | FOREGROUND_RED };
        }
        return;
    }

    if (mIsActivated)
    {
        if ((mLastUpdateSec += dtSec) < mExplosionUpdateDelay) return;
        mLastUpdateSec = 0;
        mCurMarkRadius += 1;

        const int size_x = field.CountCols(), size_y = field.CountRows();
        int x_min = max(thisCoord.X - mCurMarkRadius, 1);
        int x_max = min(thisCoord.X + mCurMarkRadius, size_x);
        int y_min = max(thisCoord.Y - mCurMarkRadius, 1);
        int y_max = min(thisCoord.Y + mCurMarkRadius, size_y);

        if (mCurMarkRadius == 0) 
        {
            mCellStyle = { L'■', BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_INTENSITY };
        }
        else if(mCurMarkRadius <= mExplosionRadius)
        {
            for (short y = y_min; y <= y_max; ++y)
            {
                for (short x = x_min; x <= x_max; ++x)
                {
                    auto curCell = field.GetCellProperties({ x, y });
                    if (curCell->IsActivable() && !curCell->TryActivate(0))
                    {
                        field.SetCellProperties(DisplayCell{ 
                            {x, y}, 
                            std::make_shared<MarkedByExplosionCell>(mExplosionRadius - mCurMarkRadius) 
                        });
                    }
                }
            }
        }
        else {
            for (short yUp = thisCoord.Y - 1; yUp >= 1; --yUp) {
                for (short x = x_min; x <= x_max; ++x)
                {
                    auto& cellAbove = field.GetCellProperties({ x, yUp });
                    if (cellAbove->IsFree()) continue;
                    cellAbove->SetFall(true);
                }
            }
        }

    }
}

inline MarkedByExplosionCell::MarkedByExplosionCell(int updatesToDestroy)
: mStyle({ L' ', BACKGROUND_RED })
, mUpdatesToDestroy(updatesToDestroy)
, mLastUpdateSec(0)
{ }
CellStyle MarkedByExplosionCell::GetCellStyle() const { return mStyle; }
bool MarkedByExplosionCell::IsReadyToDestroy() const { return mUpdatesToDestroy <= 0; }
bool MarkedByExplosionCell::IsShiftFieldDown() const { return false; }
void MarkedByExplosionCell::Update(COORD thisCoord, float dtSec, Field & field)
{
    CellProperties::Update(thisCoord, dtSec, field);

    if ((mLastUpdateSec += dtSec) < CircleExplosionCell::mExplosionUpdateDelay) return;
    mLastUpdateSec = 0;

    mUpdatesToDestroy -= 1;
}
