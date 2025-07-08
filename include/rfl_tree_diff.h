#ifndef RFL_TREE_DIFF_H
#define RFL_TREE_DIFF_H

#include <QSet>
#include <QUuid>

class FTreeDiff
{

    public:

        enum Side
        {
            A = 1,
            B
        };

        typedef struct
        {
            //! Set of ids found only in a.
            QSet<QUuid> a;
            //! Set of ids found only in b.
            QSet<QUuid> b;
            //! Set of ids found both in a and b but different.
            QSet<QUuid> ab;
        } Set;

    protected:

        Set persons;
        Set relations;

    private:

        //! Internal initialization function.
        void _init(const FTreeDiff *pTreeDiff = nullptr);

    public:

        //! Constructor.
        FTreeDiff();

        //! Copy constructor.
        FTreeDiff(const FTreeDiff &treeDiff);

        //! Destructor.
        ~FTreeDiff() {}

        //! Assignment operator.
        FTreeDiff &operator =(const FTreeDiff &treeDiff);

        //! Return if structure contains differences.
        bool containsDifferences() const;

        //! Return const reference to persons diff.
        const Set &getPersons() const;

        //! Return reference to persons diff.
        Set &getPersons();

        //! Return const reference to relations diff.
        const Set &getRelations() const;

        //! Return reference to relations diff.
        Set &getRelations();

};

#endif // RFL_TREE_DIFF_H
