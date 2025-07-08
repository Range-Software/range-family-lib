#include "../include/rfl_tree_diff.h"

void FTreeDiff::_init(const FTreeDiff *pTreeDiff)
{
    if (pTreeDiff)
    {
        this->persons = pTreeDiff->persons;
        this->relations = pTreeDiff->relations;
    }
}

FTreeDiff::FTreeDiff()
{
    this->_init();
}

FTreeDiff::FTreeDiff(const FTreeDiff &treeDiff)
{
    this->_init(&treeDiff);
}

FTreeDiff &FTreeDiff::operator =(const FTreeDiff &treeDiff)
{
    this->_init(&treeDiff);
    return (*this);
}

bool FTreeDiff::containsDifferences() const
{
    return (this->persons.a.size() ||
            this->persons.b.size() ||
            this->persons.ab.size() ||
            this->relations.a.size() ||
            this->relations.b.size() ||
            this->relations.ab.size());
}

FTreeDiff::Set &FTreeDiff::getPersons()
{
    return this->persons;
}

const FTreeDiff::Set &FTreeDiff::getPersons() const
{
    return this->persons;
}

FTreeDiff::Set &FTreeDiff::getRelations()
{
    return this->relations;
}

const FTreeDiff::Set &FTreeDiff::getRelations() const
{
    return this->relations;
}
