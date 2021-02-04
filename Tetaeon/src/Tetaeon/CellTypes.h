#pragma once
#pragma warning( disable : 4250; disable : 4099)
#include <memory>
#include <vector>
#include <functional>
#include <Windows.h>
#include "../Utility.h"
 
class CellProperties;
class ClonablePropertiesBase
{
public:
    // Deep copy
    virtual std::shared_ptr<CellProperties> clone() const = 0;
};


template <class Derived>
class ClonableProperties: virtual public ClonablePropertiesBase
{
public:
    std::shared_ptr<CellProperties> clone() const override {
        return std::make_shared<Derived>(*static_cast<Derived const*>(this));
    }
};


class DisplayCell;
class Field;
class CellProperties : public virtual ClonablePropertiesBase
{
private:
    bool mIsGravityAbove = false;
public:
    bool isGravityAbove() const;
    void SetGravityAbove(bool isGravityAbove);

    virtual bool IsFree() const;
    virtual CellStyle GetCellStyle() const = 0;
    
    // Need to activate explosions when cell may be replaced by removing line
    // Return true if activation successful
    virtual bool TryActivate(int updatesToActivate);
    virtual bool IsActivated() const;

    virtual bool IsReadyToDestroy() const;

    // Update this cell and return vector of new near cells
    virtual std::vector<DisplayCell> Update(COORD thisCoord, float dtSec, Field const& field);
};


class FreeCell : public CellProperties, public ClonableProperties<FreeCell>
{
public:
    bool IsFree() const override;
    CellStyle GetCellStyle() const override;
    static CellStyle GetCellStyleStatic();

    std::shared_ptr<CellProperties> clone() const;
};


class RegularCell : public CellProperties, public ClonableProperties<RegularCell>
{
public:
    CellStyle GetCellStyle() const override;
};


struct DisplayCell {
    COORD mPosOnField;
    std::shared_ptr<CellProperties> mProperties{ std::make_shared<RegularCell>() };
};


class HorizontalLineCell : public CellProperties, public ClonableProperties<HorizontalLineCell>
{
private:
    CellStyle mStyle = RegularCell().GetCellStyle();
    COORD mSpreadDirection;
    int mUpdatesToShown;
    int mUpdatesToDestroy;
    float mUpdateDelaySec = 0.05f;
    float mLastUpdateSec = 0.0f;
public:
    HorizontalLineCell(int updatesToShow, int updatesToDestroy);

    CellStyle GetCellStyle() const override;
    bool IsReadyToDestroy() const override;

    bool TryActivate(int updatesToActivate) override;
    bool IsActivated() const override;

    std::vector<DisplayCell> Update(COORD thisCoord, float dtSec, Field const& field) override;
};


class CellCircleExplosion: public CellProperties, public ClonableProperties<CellCircleExplosion>
{
private:
    int mExplosionRadius = 3; // Exterminate when radius equals to zero 
    bool mIsActivated = false;
    int mUpdatesToActivate = -1;
    CellStyle mCellStyle = { L'■', BACKGROUND_GREEN | FOREGROUND_RED | FOREGROUND_GREEN };

    float mUpdateInterval = 0.05f;
    float mLastUpdateSec = 0.0f;
public:

    CellStyle GetCellStyle() const override;
    bool TryActivate(int updatesToActivate) override;
    bool IsActivated() const override;

    std::vector<DisplayCell> Update(COORD thisCoord, float dtSec, Field const& field) override;
};