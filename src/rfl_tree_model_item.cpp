#include "rfl_tree_model_item.h"
#include "rfl_utils.h"

FTreeModelItem::FTreeModelItem(const QUuid &id, Type type, uint age)
    : id(id)
    , type(type)
    , age(age)
    , vRank(FConstants::eod)
    , hRank(FConstants::eod)
    , hWidth(FConstants::eod)
    , visible(false)
    , searchDirection(FConstants::Direction::Down | FConstants::Direction::Up)
{

}

const QUuid &FTreeModelItem::getId() const
{
    return this->id;
}

FTreeModelItem::Type FTreeModelItem::getType() const
{
    return this->type;
}

uint FTreeModelItem::getVRank() const
{
    return this->vRank;
}

void FTreeModelItem::setVRank(uint rank)
{
    this->vRank = rank;
}

uint FTreeModelItem::getHRank() const
{
    return this->hRank;
}

void FTreeModelItem::setHRank(uint rank)
{
    this->hRank = rank;
}

uint FTreeModelItem::getHWidth() const
{
    return this->hWidth;
}

void FTreeModelItem::setHWidth(uint hWidth)
{
    this->hWidth = hWidth;
}

bool FTreeModelItem::getVisible() const
{
    return this->visible;
}

void FTreeModelItem::setVisible(bool visible)
{
    this->visible = visible;
}

FDirectionMask FTreeModelItem::getSearchDirection() const
{
    return this->searchDirection;
}

void FTreeModelItem::setSearchDirection(FDirectionMask searchDirection)
{
    this->searchDirection = searchDirection;
}

void FTreeModelItem::addPartner(FTreeModelItem *partner)
{
    auto pos = std::lower_bound(this->partners.begin(), this->partners.end(), partner,
        [](const FTreeModelItem *a, const FTreeModelItem *b) { return a->age > b->age; });
    this->partners.insert(pos, partner);
}

const QList<FTreeModelItem*> &FTreeModelItem::getPartners() const
{
    return this->partners;
}

QList<FTreeModelItem*> &FTreeModelItem::getPartners()
{
    return this->partners;
}

void FTreeModelItem::addChild(FTreeModelItem *child)
{
    auto pos = std::lower_bound(this->children.begin(), this->children.end(), child,
        [](const FTreeModelItem *a, const FTreeModelItem *b) { return a->age > b->age; });
    this->children.insert(pos, child);
}

const QList<FTreeModelItem*> &FTreeModelItem::getChildren() const
{
    return this->children;
}

QList<FTreeModelItem*> &FTreeModelItem::getChildren()
{
    return this->children;
}

void FTreeModelItem::sortList(QList<FTreeModelItem *> &itemList)
{
    std::sort(itemList.begin(), itemList.end(),
        [](const FTreeModelItem* a, const FTreeModelItem* b) -> bool
        {
            return a->age > b->age;
        });
}
