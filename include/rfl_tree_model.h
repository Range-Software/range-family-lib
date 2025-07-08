#ifndef FL_TREE_MODEL_H
#define FL_TREE_MODEL_H

#include <QMap>
#include <QUuid>

#include "rfl_tree.h"
#include "rfl_tree_model_item.h"

class FTreeModel
{

    protected:

        //! pointer to family tree used to create a model.
        const FTree *familyTree;
        //! Tree model items mapped by their IDs.
        QMap<QUuid,FTreeModelItem*> model;
        //! Vertical rank map.
        QList< QList<const FTreeModelItem*> > vRanks;
        //! Minimum vertical rank.
        uint minVRank;
        //! Maximum vertical rank.
        uint maxVRank;
        //! Minimum horizontal rank.
        uint minHRank;
        //! Maximum horizontal rank.
        uint maxHRank;

    public:

        //! Constructor.
        FTreeModel();

        //! Constructor.
        FTreeModel(const FTree *familyTree, const QUuid &itemId);

        //! Destructor.
        ~FTreeModel();

        //! Build model from family tree.
        void build(const FTree *familyTree, const QUuid &itemId);

        //! Convert model to tree.
        //! Only visible items will be converted.
        FTree *getVisibleTree() const;

        //! Return minimum vertical rank.
        uint getMinVRank() const;

        //! Return maximum vertical rank.
        uint getMaxVRank() const;

        //! Return number of vertical ranks.
        uint getNVRanks() const;

        //! Return vertical rank position.
        uint findVRankPosition(uint rank) const;

        //! Return minimum horizontal rank.
        uint getMinHRank() const;

        //! Return maximum horizontal rank.
        uint getMaxHRank() const;

        //! Return number of horizontal ranks.
        uint getNHRanks() const;

        //! Get items for given rank position.
        const QList<const FTreeModelItem*> getItems(uint rankPosition) const;

        //! Get const reference to map.
        const QMap<QUuid,FTreeModelItem*> &getMap() const;

    protected:

        //! Convert tree to model.
        void convertTreeToModel();

        //! Set search directions.
        void calculateVisibility(const QUuid &itemId);

        //! Calculate horizontal widths.
        void calculateHWidths(const QUuid &itemId);

        //! Calculate vertical ranks.
        void calculateVRanks(const QUuid &itemId);

        //! Calculate horizontal ranks.
        void calculateHRanks(const QUuid &itemId);

};

#endif // FL_TREE_MODEL_H
