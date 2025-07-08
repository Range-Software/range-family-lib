#include <QStack>

#include "rfl_tree_model.h"
#include "rbl_error.h"
#include "rbl_logger.h"
#include "rfl_utils.h"

FTreeModel::FTreeModel()
    : familyTree{nullptr}
    , minVRank(0)
    , maxVRank(0)
    , minHRank(0)
    , maxHRank(0)
{
}

FTreeModel::FTreeModel(const FTree *familyTree, const QUuid &itemId)
    : familyTree{familyTree}
    , minVRank(0)
    , maxVRank(0)
    , minHRank(0)
    , maxHRank(0)
{
    this->build(this->familyTree,itemId);
}

FTreeModel::~FTreeModel()
{
    foreach (FTreeModelItem *item, this->model)
    {
        delete item;
    }
}

void FTreeModel::build(const FTree *familyTree, const QUuid &itemId)
{
    foreach (FTreeModelItem *item, this->model)
    {
        delete item;
    }
    this->familyTree = familyTree;
    try
    {
        this->convertTreeToModel();
        this->calculateVisibility(itemId);
        this->calculateVRanks(itemId);
        this->calculateHWidths(itemId);
        this->calculateHRanks(itemId);

        // Populate vertical ranks map
        this->vRanks.resize(this->getNVRanks());
        foreach (const FTreeModelItem *modelItem, this->model)
        {
            this->vRanks[this->findVRankPosition(modelItem->getVRank())].append(modelItem);
        }
    }
    catch (const RError &error)
    {
        RLogger::error("Failed to create a tree model. %s\n", error.getMessage().toUtf8().constData());
    }
}

FTree *FTreeModel::getVisibleTree() const
{
    FTree *visibleFamilyTree = new FTree;

    // Add persons.
    for (auto [id, modelItem] : model.asKeyValueRange())
    {
        if (!modelItem->getVisible())
        {
            continue;
        }
        if (modelItem->getType() == FTreeModelItem::Type::Person)
        {
            FPerson person = this->familyTree->findPerson(modelItem->getId());
            RLogger::debug("Adding visible person: %s\n",person.toShortString().toUtf8().constData());
            visibleFamilyTree->addPerson(person);
        }
    }
    // Add relations.
    for (auto [id, modelItem] : model.asKeyValueRange())
    {
        if (!modelItem->getVisible())
        {
            continue;
        }
        else if (modelItem->getType() == FTreeModelItem::Type::Relation)
        {
            FRelation relation = this->familyTree->findRelation(modelItem->getId());
            relation.getPartners().removeIf([visibleFamilyTree](const QUuid &id){ return !visibleFamilyTree->containsPerson(id); });
            relation.getChildren().removeIf([visibleFamilyTree](const QUuid &id){ return !visibleFamilyTree->containsPerson(id); });
            RLogger::debug("Adding visible relation: %s\n",relation.toShortString().toUtf8().constData());
            visibleFamilyTree->addRelation(relation);
        }
    }

    return visibleFamilyTree;
}

uint FTreeModel::getMinVRank() const
{
    return this->minVRank;
}

uint FTreeModel::getMaxVRank() const
{
    return this->maxVRank;
}

uint FTreeModel::getNVRanks() const
{
    return this->model.isEmpty() ? 0 : this->getMaxVRank() - this->getMinVRank() + 1;
}

uint FTreeModel::findVRankPosition(uint rank) const
{
    return rank - this->getMinVRank();
}

uint FTreeModel::getMinHRank() const
{
    return this->minHRank;
}

uint FTreeModel::getMaxHRank() const
{
    return this->maxHRank;
}

uint FTreeModel::getNHRanks() const
{
    return this->getMaxHRank() - this->getMinHRank() + 1;
}

const QList<const FTreeModelItem*> FTreeModel::getItems(uint rankPosition) const
{
    return this->vRanks[rankPosition];
}

const QMap<QUuid, FTreeModelItem *> &FTreeModel::getMap() const
{
    return this->model;
}

void FTreeModel::convertTreeToModel()
{
    QList<QUuid> personIds = this->familyTree->getPersons();
    QList<QUuid> relationIds = this->familyTree->getRelations();

    foreach (const QUuid &personId, personIds)
    {
        uint age = this->familyTree->findPerson(personId).getBirth().getDate().calculateAge();
        FTreeModelItem *item = new FTreeModelItem(personId,FTreeModelItem::Type::Person,age);
        this->model.insert(personId,item);
    }

    foreach (const QUuid &relationId, relationIds)
    {
        uint age = this->familyTree->findRelation(relationId).getBegin().getDate().calculateAge();
        FTreeModelItem *relationItem = new FTreeModelItem(relationId,FTreeModelItem::Type::Relation,age);
        this->model.insert(relationId,relationItem);

        FRelation relation = this->familyTree->findRelation(relationId);

        foreach (const QUuid &partnerId, relation.getPartners())
        {
            FTreeModelItem *partnerItem = this->model.value(partnerId,nullptr);
            if (partnerItem == nullptr)
            {
                throw RError(RError::Type::Application,R_ERROR_REF,"Non-existing key (person ID: \'%s\').",partnerId.toString(QUuid::WithoutBraces).toUtf8().constData());
            }
            partnerItem->addPartner(relationItem);
            relationItem->addPartner(partnerItem);
        }

        foreach (const QUuid &childId, relation.getChildren())
        {
            FTreeModelItem *childItem = this->model.value(childId,nullptr);
            if (childItem == nullptr)
            {
                throw RError(RError::Type::Application,R_ERROR_REF,"Non-existing key (person ID: \'%s\').",childId.toString(QUuid::WithoutBraces).toUtf8().constData());
            }
            childItem->addChild(relationItem);
            relationItem->addChild(childItem);
        }
    }
}

void FTreeModel::calculateVisibility(const QUuid &itemId)
{
    QStack<FTreeModelItem*> stack;

    foreach (FTreeModelItem *item, this->model)
    {
        if (itemId == item->getId())
        {
            item->setVisible(true);
            item->setSearchDirection(FConstants::Direction::Up | FConstants::Direction::Down);
            stack.push(item);
        }
        else
        {
            item->setVisible(false);
            item->setSearchDirection(FConstants::Direction::None);
        }
    }

    // Traverse through the tree and set correct search directions
    while (!stack.isEmpty())
    {
        FTreeModelItem *stackItem = stack.pop();

        FConstants::Direction partnerSearchDirection = stackItem->getType() == FTreeModelItem::Type::Person ? FConstants::Direction::Down : FConstants::Direction::Up;

        if (stackItem->getSearchDirection() & partnerSearchDirection)
        {
            foreach (FTreeModelItem *partnerItem, stackItem->getPartners())
            {
                if (partnerItem->getSearchDirection() == FConstants::Direction::None)
                {
                    partnerItem->setSearchDirection(partnerSearchDirection);
                    partnerItem->setVisible(true);
                    stack.push(partnerItem);
                }
            }
        }

        FConstants::Direction childSearchDirection = stackItem->getType() == FTreeModelItem::Type::Person ? FConstants::Direction::Up : FConstants::Direction::Down;

        if (stackItem->getSearchDirection() & childSearchDirection)
        {
            foreach (FTreeModelItem *childItem, stackItem->getChildren())
            {
                if (childItem->getSearchDirection() == FConstants::Direction::None)
                {
                    childItem->setSearchDirection(childSearchDirection);
                    childItem->setVisible(true);
                    stack.push(childItem);
                }
            }
        }
    }

    // // Make partners visible
    // foreach (FTreeModelItem *item, this->model)
    // {
    //     if (itemId == item->getId())
    //     {
    //         if (item->getSearchDirection() & FConstants::Direction::Down)
    //         {
    //             foreach (FTreeModelItem *partnerItem, item->getPartners())
    //             {
    //                 if (partnerItem->getType() == FTreeModelItem::Type::Relation && partnerItem->getVisible())
    //                 {
    //                     foreach (FTreeModelItem *partnerItem, partnerItem->getPartners())
    //                     {
    //                         partnerItem->setSearchDirection(FConstants::Direction::None);
    //                         partnerItem->setVisible(true);
    //                     }
    //                 }
    //             }
    //         }

    //         break;
    //     }
    // }
}

void FTreeModel::calculateHWidths(const QUuid &itemId)
{
    if (!this->model.contains(itemId))
    {
        return;
    }

    QStack<FTreeModelItem*> stack;

    FTreeModelItem *item = this->model[itemId];

    if (!item->getVisible() || item->getHWidth() != FConstants::eod)
    {
        return;
    }

    stack.push(item);

    // Traverse through the tree and set correct withs
    while (!stack.isEmpty())
    {
        FTreeModelItem *stackItem = stack.top();

        bool popItem = true;

        uint upHWidth = 0;
        uint downHWidth = 0;

        stackItem->setHWidth(0);

        if (stackItem->getType() == FTreeModelItem::Person)
        {
            if (stackItem->getSearchDirection() & FConstants::Direction::Up)
            {
                foreach (FTreeModelItem *childItem, stackItem->getChildren())
                {
                    if (childItem->getHWidth() == FConstants::eod)
                    {
                        stack.push(childItem);
                        popItem = false;
                    }
                    else
                    {
                        upHWidth += childItem->getHWidth();
                    }
                }
            }
            if (stackItem->getSearchDirection() & FConstants::Direction::Down)
            {
                foreach (FTreeModelItem *partnerItem, stackItem->getPartners())
                {
                    if (partnerItem->getHWidth() == FConstants::eod)
                    {
                        stack.push(partnerItem);
                        popItem = false;
                    }
                    else
                    {
                        downHWidth += partnerItem->getHWidth();
                    }
                }
            }
        }
        else if (stackItem->getType() == FTreeModelItem::Relation)
        {
            if (stackItem->getSearchDirection() & FConstants::Direction::Up)
            {
                foreach (FTreeModelItem *partnerItem, stackItem->getPartners())
                {
                    if (partnerItem->getHWidth() == FConstants::eod)
                    {
                        stack.push(partnerItem);
                        popItem = false;
                    }
                    else
                    {
                        upHWidth += partnerItem->getHWidth();
                    }
                }
            }
            if (stackItem->getSearchDirection() & FConstants::Direction::Down)
            {
                foreach (FTreeModelItem *childItem, stackItem->getChildren())
                {
                    if (childItem->getHWidth() == FConstants::eod)
                    {
                        stack.push(childItem);
                        popItem = false;
                    }
                    else
                    {
                        downHWidth += childItem->getHWidth();
                    }
                }
            }
        }

        if (popItem)
        {
            stackItem->setHWidth(std::max(std::max(upHWidth,downHWidth),1u));
            stack.pop();
        }
    }
}

//! Calculate vertical ranks.
void FTreeModel::calculateVRanks(const QUuid &itemId)
{
    if (!this->model.contains(itemId))
    {
        return;
    }

    QStack<FTreeModelItem*> stack;

    FTreeModelItem *item = this->model[itemId];

    if (!item->getVisible() || item->getHRank() != FConstants::eod)
    {
        return;
    }

    stack.push(item);

    // Traverse through the tree and set correct ranks
    while (!stack.isEmpty())
    {
        FTreeModelItem *stackItem = stack.pop();

        if (stackItem->getVRank() == FConstants::eod)
        {
            stackItem->setVRank(FConstants::eod / 2);
        }

        int partnerRankIncrease = stackItem->getType() == FTreeModelItem::Type::Person ? 1 : -1;
        foreach (FTreeModelItem *partnerItem, stackItem->getPartners())
        {
            if (partnerItem->getVRank() == FConstants::eod && partnerItem->getVisible())
            {
                partnerItem->setVRank(stackItem->getVRank() + partnerRankIncrease);
                stack.push(partnerItem);
            }
        }

        int childRankIncrease = stackItem->getType() == FTreeModelItem::Type::Person ? -1 : 1;
        foreach (FTreeModelItem *childItem, stackItem->getChildren())
        {
            if (childItem->getVRank() == FConstants::eod && childItem->getVisible())
            {
                childItem->setVRank(stackItem->getVRank() + childRankIncrease);
                stack.push(childItem);
            }
        }
    }

    // Calculate minimum and maximum ranks.
    bool isFirst = true;
    foreach (FTreeModelItem *item, this->model)
    {
        uint vRank = item->getVRank();

        if (vRank == FConstants::eod)
        {
            this->model.remove(item->getId());
        }
        else
        {
            if (isFirst)
            {
                this->minVRank = vRank;
                this->maxVRank = vRank;
                isFirst = false;
            }
            else
            {
                this->minVRank = std::min(this->minVRank,vRank);
                this->maxVRank = std::max(this->maxVRank,vRank);
            }
        }
    }
}

//! Calculate horizontal ranks.
void FTreeModel::calculateHRanks(const QUuid &itemId)
{
    if (!this->model.contains(itemId))
    {
        return;
    }

    QStack<FTreeModelItem*> stack;

    FTreeModelItem *item = this->model[itemId];

    if (!item->getVisible() || item->getHRank() != FConstants::eod)
    {
        return;
    }

    if (item->getHRank() == FConstants::eod)
    {
        item->setHRank(FConstants::eod / 2);
    }

    stack.push(item);

    // Traverse through the tree and set correct ranks
    while (!stack.isEmpty())
    {
        FTreeModelItem *stackItem = stack.pop();

        // Process children
        uint childrenWidth = 0;
        foreach (FTreeModelItem *childItem, stackItem->getChildren())
        {
            childrenWidth += childItem->getHWidth();
        }

        uint childLowHRank = stackItem->getHRank() - childrenWidth / 2;
        foreach (FTreeModelItem *childItem, stackItem->getChildren())
        {
            if (childItem->getHRank() == FConstants::eod && childItem->getVisible())
            {
                childItem->setHRank(childLowHRank + childItem->getHWidth()/2);
                childLowHRank += childItem->getHWidth();
                stack.push(childItem);
            }
        }

        // Process partners
        uint partnersWidth = 0;
        foreach (FTreeModelItem *partnerItem, stackItem->getPartners())
        {
            partnersWidth += partnerItem->getHWidth();
        }

        uint partnerLowHRank = stackItem->getHRank() - partnersWidth / 2;
        foreach (FTreeModelItem *partnerItem, stackItem->getPartners())
        {
            if (partnerItem->getHRank() == FConstants::eod && partnerItem->getVisible())
            {
                partnerItem->setHRank(partnerLowHRank + partnerItem->getHWidth()/2);
                partnerLowHRank += partnerItem->getHWidth();
                stack.push(partnerItem);
            }
        }
    }

    // Calculate minimum and maximum ranks.
    bool isFirst = true;
    foreach (FTreeModelItem *item, this->model)
    {
        uint hRank = item->getHRank();

        if (hRank == FConstants::eod)
        {
            this->model.remove(item->getId());
        }
        else
        {
            if (isFirst)
            {
                this->minHRank = hRank;
                this->maxHRank = hRank;
                isFirst = false;
            }
            else
            {
                this->minHRank = std::min(this->minHRank,hRank);
                this->maxHRank = std::max(this->maxHRank,hRank);
            }
        }
    }
}
