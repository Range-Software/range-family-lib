#include <QList>
#include <QSet>
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
    this->model.clear();
    this->vRanks.clear();
    this->minVRank = this->maxVRank = this->minHRank = this->maxHRank = 0;
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
        const FRelation relation = this->familyTree->findRelation(relationId);
        uint age = relation.getBegin().getDate().calculateAge();
        FTreeModelItem *relationItem = new FTreeModelItem(relationId,FTreeModelItem::Type::Relation,age);
        this->model.insert(relationId,relationItem);

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

    if (stack.isEmpty())
    {
        RLogger::warning("Tree model: item not found in model (ID: '%s').\n",
                         itemId.toString(QUuid::WithoutBraces).toUtf8().constData());
        return;
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

    // Phase 1: collect visible items in post-order (dependencies before dependants).
    // A nullptr sentinel on the stack means "the item below is ready to collect".
    QList<FTreeModelItem*> postOrder;
    QSet<FTreeModelItem*> scheduled;
    scheduled.insert(item);
    stack.push(item);

    while (!stack.isEmpty())
    {
        FTreeModelItem *cur = stack.pop();

        if (cur == nullptr)
        {
            // Sentinel: item immediately below on the stack is fully resolved.
            postOrder.append(stack.pop());
            continue;
        }

        // Re-push cur with a sentinel so it is collected after its dependencies.
        stack.push(cur);
        stack.push(nullptr);

        // Push each unscheduled visible dependency so it is resolved first.
        auto scheduleDeps = [&](const QList<FTreeModelItem*> &deps)
        {
            for (FTreeModelItem *dep : deps)
            {
                if (dep->getVisible() && !scheduled.contains(dep))
                {
                    scheduled.insert(dep);
                    stack.push(dep);
                }
            }
        };

        if (cur->getType() == FTreeModelItem::Person)
        {
            if (cur->getSearchDirection() & FConstants::Direction::Up)
                scheduleDeps(cur->getChildren());
            if (cur->getSearchDirection() & FConstants::Direction::Down)
                scheduleDeps(cur->getPartners());
        }
        else if (cur->getType() == FTreeModelItem::Relation)
        {
            if (cur->getSearchDirection() & FConstants::Direction::Up)
                scheduleDeps(cur->getPartners());
            if (cur->getSearchDirection() & FConstants::Direction::Down)
                scheduleDeps(cur->getChildren());
        }
    }

    // Phase 2: compute widths bottom-up; every dependency is already computed.
    for (FTreeModelItem *cur : postOrder)
    {
        uint upHWidth = 0;
        uint downHWidth = 0;

        if (cur->getType() == FTreeModelItem::Person)
        {
            if (cur->getSearchDirection() & FConstants::Direction::Up)
            {
                for (FTreeModelItem *child : cur->getChildren())
                    if (child->getVisible()) upHWidth += child->getHWidth();
            }
            if (cur->getSearchDirection() & FConstants::Direction::Down)
            {
                for (FTreeModelItem *partner : cur->getPartners())
                    if (partner->getVisible()) downHWidth += partner->getHWidth();
            }
        }
        else if (cur->getType() == FTreeModelItem::Relation)
        {
            if (cur->getSearchDirection() & FConstants::Direction::Up)
            {
                for (FTreeModelItem *partner : cur->getPartners())
                    if (partner->getVisible()) upHWidth += partner->getHWidth();
            }
            if (cur->getSearchDirection() & FConstants::Direction::Down)
            {
                for (FTreeModelItem *child : cur->getChildren())
                    if (child->getVisible()) downHWidth += child->getHWidth();
            }
        }

        cur->setHWidth(std::max(std::max(upHWidth, downHWidth), 1u));
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

    if (!item->getVisible() || item->getVRank() != FConstants::eod)
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

    // Calculate minimum and maximum ranks, pruning unranked items.
    bool isFirst = true;
    for (auto it = this->model.begin(); it != this->model.end(); )
    {
        FTreeModelItem *item = it.value();
        uint vRank = item->getVRank();

        if (vRank == FConstants::eod)
        {
            delete item;
            it = this->model.erase(it);
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
            ++it;
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

    item->setHRank(FConstants::eod / 2);

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

        int childLowHRank = static_cast<int>(stackItem->getHRank()) - static_cast<int>(childrenWidth) / 2;
        foreach (FTreeModelItem *childItem, stackItem->getChildren())
        {
            if (childItem->getHRank() == FConstants::eod && childItem->getVisible())
            {
                childItem->setHRank(static_cast<uint>(childLowHRank + static_cast<int>(childItem->getHWidth()) / 2));
                childLowHRank += static_cast<int>(childItem->getHWidth());
                stack.push(childItem);
            }
        }

        // Process partners
        uint partnersWidth = 0;
        foreach (FTreeModelItem *partnerItem, stackItem->getPartners())
        {
            partnersWidth += partnerItem->getHWidth();
        }

        int partnerLowHRank = static_cast<int>(stackItem->getHRank()) - static_cast<int>(partnersWidth) / 2;
        foreach (FTreeModelItem *partnerItem, stackItem->getPartners())
        {
            if (partnerItem->getHRank() == FConstants::eod && partnerItem->getVisible())
            {
                partnerItem->setHRank(static_cast<uint>(partnerLowHRank + static_cast<int>(partnerItem->getHWidth()) / 2));
                partnerLowHRank += static_cast<int>(partnerItem->getHWidth());
                stack.push(partnerItem);
            }
        }
    }

    // Calculate minimum and maximum ranks, pruning unranked items.
    bool isFirst = true;
    for (auto it = this->model.begin(); it != this->model.end(); )
    {
        FTreeModelItem *item = it.value();
        uint hRank = item->getHRank();

        if (hRank == FConstants::eod)
        {
            delete item;
            it = this->model.erase(it);
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
            ++it;
        }
    }
}
