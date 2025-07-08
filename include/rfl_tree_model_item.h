#ifndef FL_TREE_MODEL_ITEM_H
#define FL_TREE_MODEL_ITEM_H

#include <QUuid>
#include <QList>

#include "rfl_utils.h"

class FTreeModelItem
{

    public:

        enum Type
        {
            Person = 0,
            Relation,
            NTypes
        };

    protected:

        //! ID.
        QUuid id;
        //! Item type.
        Type type;
        //! Age in days.
        uint age;
        //! Vertical rank.
        uint vRank;
        //! Horizontal rank.
        uint hRank;
        //! Horizontal width.
        uint hWidth;
        //! Item is visible.
        bool visible;
        //! Search direction.
        FDirectionMask searchDirection;
        //! List of partner links.
        QList<FTreeModelItem*> partners;
        //! List of partner links.
        QList<FTreeModelItem*> children;

    public:

        //! Constructor.
        FTreeModelItem(const QUuid &id,Type type,uint age);

        //! Get id.
        const QUuid &getId() const;

        //! Get type.
        Type getType() const;

        //! Get vertical rank.
        uint getVRank() const;

        //! Set vertical rank.
        void setVRank(uint rank);

        //! Get horizontal rank.
        uint getHRank() const;

        //! Set horizontal rank.
        void setHRank(uint rank);

        //! Get horizontal width.
        uint getHWidth() const;

        //! Set horizontal width.
        void setHWidth(uint hWidth);

        //! Get visible.
        bool getVisible() const;

        //! Set visible.
        void setVisible(bool visible);

        //! Get search direction.
        FDirectionMask getSearchDirection() const;

        //! Set search direction.
        void setSearchDirection(FDirectionMask searchDirection);

        //! Add partner link.
        void addPartner(FTreeModelItem *partner);

        //! Get list of partners.
        const QList<FTreeModelItem*> &getPartners() const;

        //! Get list of partners.
        QList<FTreeModelItem*> &getPartners();

        //! Add child link.
        void addChild(FTreeModelItem *child);

        //! Get list of children.
        const QList<FTreeModelItem*> &getChildren() const;

        //! Get list of children.
        QList<FTreeModelItem*> &getChildren();

        static void sortList(QList<FTreeModelItem*> &itemList);

};

#endif // FL_TREE_MODEL_ITEM_H
