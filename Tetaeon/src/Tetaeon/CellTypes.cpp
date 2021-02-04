#include "CellTypes.h"
#include "Field.h"

bool CellProperties::isGravityAbove() const { return mIsGravityAbove; }
void CellProperties::SetGravityAbove(bool isGravityAbove) { mIsGravityAbove = isGravityAbove; }
bool CellProperties::IsFree() const { return false; }
bool CellProperties::TryActivate(int updatesToActivate) { return false; }
bool CellProperties::IsActivated() const { return false; }
bool CellProperties::IsReadyToDestroy() const { return false; }
std::vector<DisplayCell> CellProperties::Update(COORD thisCoord, float dtSec, Field const & field)
{
    return {};
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
std::shared_ptr<CellProperties> FreeCell::clone() const {
    return std::make_shared<FreeCell>(*static_cast<FreeCell const*>(this));
}


CellStyle RegularCell::GetCellStyle() const { return { L'█', FOREGROUND_GREEN }; }


HorizontalLineCell::HorizontalLineCell(int updatesToShow, int updatesToDestroy)
: mUpdatesToShown(updatesToShow)
, mUpdatesToDestroy(updatesToDestroy)
{
    if (mUpdatesToShown == 0) {
        mStyle = { L'─', BACKGROUND_GREEN | FOREGROUND_RED };
    }
}
CellStyle HorizontalLineCell::GetCellStyle() const { return mStyle; }
bool HorizontalLineCell::IsReadyToDestroy() const { return mUpdatesToDestroy == 0; }
bool HorizontalLineCell::TryActivate(int updatesToActivate) { return true; }
bool HorizontalLineCell::IsActivated() const { return true; }
std::vector<DisplayCell> HorizontalLineCell::Update(COORD thisCoord, float dtSec, Field const & field)
{
    mLastUpdateSec += dtSec;
    if (mLastUpdateSec >= mUpdateDelaySec) {
        mLastUpdateSec = 0.0f;
        mUpdatesToShown -= 1;
        mUpdatesToDestroy -= 1;

        if (mUpdatesToShown == 0) {
            mStyle = { L'─', BACKGROUND_GREEN | FOREGROUND_RED };
        }
    }
    return { };
}


CellStyle CellCircleExplosion::GetCellStyle() const { return mCellStyle; }
bool CellCircleExplosion::TryActivate(int updatesToActivate)
{
    mUpdatesToActivate = updatesToActivate;
    return true;
}
bool CellCircleExplosion::IsActivated() const { return mIsActivated or mUpdatesToActivate > 0; }
std::vector<DisplayCell> CellCircleExplosion::Update(COORD thisCoord, float dtSec, Field const & field)
{
    if ((mLastUpdateSec += dtSec) < mUpdateInterval) return { };
    mLastUpdateSec = 0;

    bool isActivateNow = mUpdatesToActivate == 0;
    bool isActivateLater = mUpdatesToActivate > 0;
    if (!mIsActivated and (isActivateNow or isActivateLater and --mUpdatesToActivate == 0))
    {
        mIsActivated = true;
        mCellStyle = { L'■', BACKGROUND_GREEN | FOREGROUND_RED };
    }

    return {};
}
