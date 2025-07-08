#include <rbl_error.h>
#include <rbl_logger.h>

#include "rfl_tool_action.h"

Q_DECLARE_METATYPE(FPerson);
Q_DECLARE_METATYPE(FRelation);

void FToolAction::_init(const FToolAction *pFToolAction)
{
    R_LOG_TRACE_IN;
    if (pFToolAction)
    {
        this->type = pFToolAction->type;
        this->input = pFToolAction->input;
        this->pTree = pFToolAction->pTree;
    }
    R_LOG_TRACE_OUT;
}

FToolAction::FToolAction(Type type, FTree *pTree)
    : type(type)
    , pTree(pTree)
{
    R_LOG_TRACE_IN;
    this->_init();
    R_LOG_TRACE_OUT;
}

FToolAction::FToolAction(const FToolAction &toolAction)
    : RToolAction(toolAction)
{
    R_LOG_TRACE_IN;
    this->_init(&toolAction);
    R_LOG_TRACE_OUT;
}

FToolAction::~FToolAction()
{
    R_LOG_TRACE_IN;
    R_LOG_TRACE_OUT;
}

FToolAction &FToolAction::operator =(const FToolAction &toolAction)
{
    R_LOG_TRACE_IN;
    this->_init(&toolAction);
    R_LOG_TRACE_RETURN(*this);
}

void FToolAction::perform()
{
    R_LOG_TRACE_IN;

    switch (this->type)
    {
        case ReadTreeFile:
        {
            const QString &fileName = this->input.toString();
            this->pTree->blockSignals(true);
            this->pTree->readFile(fileName);
            this->pTree->blockSignals(false);
            emit this->pTree->fileLoaded(fileName);
            RLogger::info("Tree has been successfully loaded from file \'%s\'.\n", fileName.toUtf8().constData());
            break;
        }
        case WriteTreeFile:
        {
            const QString &fileName = this->input.toString();
            this->pTree->writeFile(fileName);
            RLogger::info("Tree has been successfully saved to file \'%s\'.\n", fileName.toUtf8().constData());
            break;
        }
        case DiffTreeFiles:
        {
            const QStringList &fileNames = this->input.toStringList();
            if (fileNames.count() != 2)
            {
                throw RError(RError::Type::InvalidInput,R_ERROR_REF,
                             "Invalid number of files to be compared. Must be \'2\', but \'%l\' were provided.",
                             fileNames.count());
            }
            FTreeDiff diff = FTree::diffFiles(fileNames.at(0),fileNames.at(1));
            RLogger::info("Diff between files a=\'%s\' and b=\'%s\' was produced.\n",
                          fileNames.at(0).toUtf8().constData(),
                          fileNames.at(1).toUtf8().constData());
            emit this->pTree->diffProduced(fileNames.at(0),fileNames.at(1),diff);
            break;
        }
        case GenerateDot:
        {
            const QString &fileName = this->input.toString();
            this->pTree->writeDotFile(fileName);
            RLogger::info("Tree has been successfully exported to file \'%s\'.\n", fileName.toUtf8().constData());
            break;
        }
        case AddPerson:
        {
            const FPerson &person = qvariant_cast<FPerson>(this->input);
            person.validate();
            this->pTree->addPerson(person);
            break;
        }
        case RemovePerson:
        {
            const QUuid &id = this->input.toUuid();
            this->pTree->removePerson(id,true);
            break;
        }
        case ModifyPerson:
        {
            const FPerson &person = qvariant_cast<FPerson>(this->input);
            person.validate();
            this->pTree->setPerson(person);
            break;
        }
        case PrintPerson:
        {
            const QUuid &id = this->input.toUuid();
            FPerson person = this->pTree->findPerson(id);
            if (person.isEmpty())
            {
                throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Non-existing key (person ID: \'%s\').",id.toString(QUuid::WithoutBraces).toUtf8().constData());
            }
            else
            {
                RLogger::info("Person:\n");
                RLogger::indent();
                person.print();
                RLogger::unindent(false);
            }
            break;
        }
        case ListPersons:
        {
            RLogger::info("Persons:\n");
            RLogger::indent();
            const QString &sortPath = this->input.toString();
            this->pTree->printPersonsList(sortPath);
            RLogger::unindent(false);
            break;
        }
        case AddRelation:
        {
            const FRelation &relation = qvariant_cast<FRelation>(this->input);
            relation.validate();
            this->pTree->addRelation(relation);
            break;
        }
        case RemoveRelation:
        {
            const QUuid &id = this->input.toUuid();
            this->pTree->removeRelation(id);
            break;
        }
        case ModifyRelation:
        {
            const FRelation &relation = qvariant_cast<FRelation>(this->input);
            relation.validate();
            this->pTree->setRelation(relation);
            break;
        }
        case PrintRelation:
        {
            const QUuid &id = this->input.toUuid();
            FRelation relation = this->pTree->findRelation(id);
            if (relation.isEmpty())
            {
                throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Non-existing key (relation ID: \'%s\').",id.toString(QUuid::WithoutBraces).toUtf8().constData());
            }
            else
            {
                RLogger::info("Relation:\n");
                RLogger::indent();
                relation.print();
                RLogger::unindent(false);
            }
            break;
        }
        case ListRelations:
        {
            RLogger::info("Relations:\n");
            RLogger::indent();
            const QString &sortPath = this->input.toString();
            this->pTree->printRelationsList(sortPath);
            RLogger::unindent(false);
            break;
        }
        default:
        {
            break;
        }
    }
    R_LOG_TRACE_OUT;
}

QSharedPointer<FToolAction> FToolAction::readTreeFile(FTree *pTree, const QString &fileName)
{
    FToolAction *toolAction = new FToolAction(ReadTreeFile,pTree);

    toolAction->input.setValue(fileName);

    return QSharedPointer<FToolAction>(toolAction);
}

QSharedPointer<FToolAction> FToolAction::writeTreeFile(FTree *pTree, const QString &fileName)
{
    FToolAction *toolAction = new FToolAction(WriteTreeFile,pTree);

    toolAction->input.setValue(fileName);

    return QSharedPointer<FToolAction>(toolAction);
}

QSharedPointer<FToolAction> FToolAction::diffTreeFiles(FTree *pTree, const QString &aFileName, const QString &bFileName)
{
    FToolAction *toolAction = new FToolAction(DiffTreeFiles,pTree);

    QStringList fileNames;
    fileNames << aFileName;
    fileNames << bFileName;
    toolAction->input.setValue(fileNames);

    return QSharedPointer<FToolAction>(toolAction);
}

QSharedPointer<FToolAction> FToolAction::generateDot(FTree *pTree, const QString &fileName)
{
    FToolAction *toolAction = new FToolAction(GenerateDot,pTree);

    toolAction->input.setValue(fileName);

    return QSharedPointer<FToolAction>(toolAction);
}

QSharedPointer<FToolAction> FToolAction::addPerson(FTree *pTree, const FPerson &person)
{
    FToolAction *toolAction = new FToolAction(AddPerson,pTree);

    toolAction->input.setValue(person);

    return QSharedPointer<FToolAction>(toolAction);
}

QSharedPointer<FToolAction> FToolAction::removePerson(FTree *pTree, const QUuid &id)
{
    FToolAction *toolAction = new FToolAction(RemovePerson,pTree);

    toolAction->input.setValue(id);

    return QSharedPointer<FToolAction>(toolAction);
}

QSharedPointer<FToolAction> FToolAction::modifyPerson(FTree *pTree, const FPerson &person)
{
    FToolAction *toolAction = new FToolAction(ModifyPerson,pTree);

    toolAction->input.setValue(person);

    return QSharedPointer<FToolAction>(toolAction);
}

QSharedPointer<FToolAction> FToolAction::printPerson(FTree *pTree, const QUuid &id)
{
    FToolAction *toolAction = new FToolAction(PrintPerson,pTree);

    toolAction->input.setValue(id);

    return QSharedPointer<FToolAction>(toolAction);
}

QSharedPointer<FToolAction> FToolAction::listPersons(FTree *pTree, const QString &sortPath)
{
    FToolAction *toolAction = new FToolAction(ListPersons,pTree);

    toolAction->input.setValue(sortPath);

    return QSharedPointer<FToolAction>(toolAction);
}

QSharedPointer<FToolAction> FToolAction::addRelation(FTree *pTree, const FRelation &relation)
{
    FToolAction *toolAction = new FToolAction(AddRelation,pTree);

    toolAction->input.setValue(relation);

    return QSharedPointer<FToolAction>(toolAction);
}

QSharedPointer<FToolAction> FToolAction::removeRelation(FTree *pTree, const QUuid &id)
{
    FToolAction *toolAction = new FToolAction(RemoveRelation,pTree);

    toolAction->input.setValue(id);

    return QSharedPointer<FToolAction>(toolAction);
}

QSharedPointer<FToolAction> FToolAction::modifyRelation(FTree *pTree, const FRelation &relation)
{
    FToolAction *toolAction = new FToolAction(ModifyRelation,pTree);

    toolAction->input.setValue(relation);

    return QSharedPointer<FToolAction>(toolAction);
}

QSharedPointer<FToolAction> FToolAction::printRelation(FTree *pTree, const QUuid &id)
{
    FToolAction *toolAction = new FToolAction(PrintRelation,pTree);

    toolAction->input.setValue(id);

    return QSharedPointer<FToolAction>(toolAction);
}

QSharedPointer<FToolAction> FToolAction::listRelations(FTree *pTree, const QString &sortPath)
{
    FToolAction *toolAction = new FToolAction(ListRelations,pTree);

    toolAction->input.setValue(sortPath);

    return QSharedPointer<FToolAction>(toolAction);
}

