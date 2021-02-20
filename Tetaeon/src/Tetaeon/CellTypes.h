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
class ClonableProperties : virtual public ClonablePropertiesBase
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
    bool mIsFall = false;
    float mFallInterval = 0.02f;
    float mLastFall = 0;

public:
    void SetFall(bool isFall);

    virtual bool IsFree() const;
    virtual CellStyle GetCellStyle() const = 0;

    // Need to activate explosions when cell may be replaced by removing line
    // Or for mark cell as in deal with remove process 
    // Return true if activation successful
    virtual bool TryActivate(int updatesToActivate);
    virtual bool IsActivated() const;
    bool IsActivable() const;

    virtual bool IsReadyToDestroy() const;
    virtual bool IsShiftFieldDown() const;

    virtual void Update(COORD thisCoord, float dtSec, Field& field);
};


class FreeCell : public CellProperties, public ClonableProperties<FreeCell>
{
public:
    bool IsFree() const override;
    CellStyle GetCellStyle() const override;
    static CellStyle GetCellStyleStatic();

    std::shared_ptr<CellProperties> clone() const;
};


class FigureCell : public CellProperties, public ClonableProperties<FigureCell>
{
public:
    CellStyle GetCellStyle() const override;
};


struct DisplayCell {
    COORD mPosOnField;
    std::shared_ptr<CellProperties> mProperties{ std::make_shared<FigureCell>() };
};


class HorizontalLineCell : public CellProperties, public ClonableProperties<HorizontalLineCell>
{
    friend class CircleExplosionCell;
private:
    CellStyle mStyle;
    int mUpdatesToShown;
    int mUpdatesToDestroy;
    static float mUpdateDelaySec;
    float mLastUpdateSec;

public:
    HorizontalLineCell(int updatesToShow, int updatesToDestroy);

    CellStyle GetCellStyle() const override;
    bool IsReadyToDestroy() const override;

    bool TryActivate(int updatesToActivate) override;
    bool IsActivated() const override;

    void Update(COORD thisCoord, float dtSec, Field& field) override;
};


class CircleExplosionCell: public CellProperties, public ClonableProperties<CircleExplosionCell>
{
    friend class MarkedByExplosionCell;
private:
    static float mExplosionUpdateDelay;
    static int mExplosionRadius; // Exterminate when radius equals to zero 
    int mCurMarkRadius;
    float mLastUpdateSec;

    bool mIsActivated;
    int mUpdatesToActivate;
    CellStyle mCellStyle;

public:
    CircleExplosionCell();
    CellStyle GetCellStyle() const override;
    bool TryActivate(int updatesToActivate) override;
    bool IsActivated() const override;
    bool IsReadyToDestroy() const override;
    bool IsShiftFieldDown() const override;
    void Update(COORD thisCoord, float dtSec, Field& field) override;
};


class MarkedByExplosionCell: public CellProperties, public ClonableProperties<MarkedByExplosionCell> 
{
private:
    CellStyle mStyle;
    int mUpdatesToDestroy;
    float mLastUpdateSec;

public:
    MarkedByExplosionCell(int updatesToDestroy);
    CellStyle GetCellStyle() const override;
    bool IsActivated() const { return true; }
    bool TryActivate() { return true; }
    bool IsReadyToDestroy() const override;
    bool IsShiftFieldDown() const override;
    void Update(COORD thisCoord, float dtSec, Field& field) override;
};