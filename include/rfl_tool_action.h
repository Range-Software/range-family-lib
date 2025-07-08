#ifndef FL_TOOL_ACTION_H
#define FL_TOOL_ACTION_H

#include <QVariant>
#include <QSharedPointer>

#include <rbl_tool_action.h>

#include "rfl_tree.h"

class FToolAction : public RToolAction
{

    protected:

        enum Type
        {
            ReadTreeFile = 0,
            WriteTreeFile,
            DiffTreeFiles,
            GenerateDot,
            AddPerson,
            RemovePerson,
            ModifyPerson,
            PrintPerson,
            ListPersons,
            AddRelation,
            RemoveRelation,
            ModifyRelation,
            PrintRelation,
            ListRelations,
            NTypes
        };

        //! Action type.
        Type type;
        //! Family tree.
        FTree *pTree;

    private:

        //! Internal initialization function.
        void _init(const FToolAction *pFToolAction = nullptr);

    public:

        //! Constructor.
        FToolAction(Type type, FTree *pTree);

        //! Copy constructor.
        FToolAction(const FToolAction &toolAction);

        //! Destructor.
        ~FToolAction();

        //! Assignment operator.
        FToolAction & operator =(const FToolAction &toolAction);

        //! Perform action.
        void perform() override;

        //! Set action read XML file.
        static QSharedPointer<FToolAction> readTreeFile(FTree *pTree, const QString &fileName);

        //! Set action write XML file.
        static QSharedPointer<FToolAction> writeTreeFile(FTree *pTree, const QString &fileName);

        //! Set action compare XML files.
        static QSharedPointer<FToolAction> diffTreeFiles(FTree *pTree, const QString &aFileName,const QString &bFileName);

        //! Set action generate dot (Graphviz) file.
        static QSharedPointer<FToolAction> generateDot(FTree *pTree, const QString &fileName);

        //! Set action add person.
        static QSharedPointer<FToolAction> addPerson(FTree *pTree, const FPerson &person);

        //! Set action remove person.
        static QSharedPointer<FToolAction> removePerson(FTree *pTree, const QUuid &id);

        //! Set action modify person.
        static QSharedPointer<FToolAction> modifyPerson(FTree *pTree, const FPerson &person);

        //! Set action print person.
        static QSharedPointer<FToolAction> printPerson(FTree *pTree, const QUuid &id);

        //! Set action list persons.
        static QSharedPointer<FToolAction> listPersons(FTree *pTree, const QString &sortPath);

        //! Set action add relation.
        static QSharedPointer<FToolAction> addRelation(FTree *pTree, const FRelation &relation);

        //! Set action remove relation.
        static QSharedPointer<FToolAction> removeRelation(FTree *pTree, const QUuid &id);

        //! Set action modify relation.
        static QSharedPointer<FToolAction> modifyRelation(FTree *pTree, const FRelation &relation);

        //! Set action print relation.
        static QSharedPointer<FToolAction> printRelation(FTree *pTree, const QUuid &id);

        //! Set action list relations.
        static QSharedPointer<FToolAction> listRelations(FTree *pTree, const QString &sortPath);

};

#endif // FL_TOOL_ACTION_H
