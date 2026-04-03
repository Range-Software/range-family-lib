#ifndef FL_TREE_H
#define FL_TREE_H

#include <QObject>
#include <QUuid>
#include <QMap>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <rbl_version.h>

#include "rfl_person.h"
#include "rfl_relation.h"
#include "rfl_tree_diff.h"

//! Volume element group class
class FTree : public QObject
{

    Q_OBJECT

    public:

        struct ElementName
        {
            static const QString tree;
            static const QString version;
            static const QString persons;
            static const QString relations;
            static const QString person;
            static const QString relation;
        };
        struct ElementDesc
        {
            static const QString tree;
            static const QString version;
            static const QString persons;
            static const QString relations;
            static const QString person;
            static const QString relation;
        };
        //! File version.
        static const RVersion version;

    protected:

        //! Map of persons
        QMap<QUuid,FPerson> persons;
        //! Map of relations
        QMap<QUuid,FRelation> relations;

    public:

        //! Constructor.
        explicit FTree(QObject *parent = nullptr);

        //! Add person.
        void addPerson(const FPerson &person);

        //! Set person.
        void setPerson(const FPerson &person);

        //! Update person.
        void updatePerson(const FPerson &person);

        //! Remove person.
        void removePerson(const QUuid &id, bool force);

        //! Find person.
        FPerson findPerson(const QUuid &id) const;

        //! Check if tree contains given person.
        bool containsPerson(const QUuid &id) const;

        //! Add relation.
        void addRelation(const FRelation &relation);

        //! Set relation.
        void setRelation(const FRelation &relation);

        //! Update relation.
        void updateRelation(const FRelation &relation);

        //! Remove relation.
        void removeRelation(const QUuid &id);

        //! Find relation.
        FRelation findRelation(const QUuid &id) const;

        //! Check if tree contains given relation.
        bool containsRelation(const QUuid &id) const;

        //! Clear tree.
        void clear();

        //! Get persons IDs list.
        QList<QUuid> getPersons(const QString &sortPath = QString()) const;

        //! Get relations IDs list.
        QList<QUuid> getRelations(const QString &sortPath = QString()) const;

        //! Find all relations for given person.
        QList<QUuid> findPersonRelations(const QUuid &id) const;

        //! Find all relations for given person as partner.
        QList<QUuid> findPersonPartnerRelations(const QUuid &id) const;

        //! Find all relations for given person as child.
        QList<QUuid> findPersonChildRelations(const QUuid &id) const;

        //! Find all persons for given relation.
        QList<QUuid> findRelationPersons(const QUuid &id) const;

        //! Sort person list.
        QList<QUuid> sortPersons(const QList<QUuid> &ids) const;

        //! Merge persons.
        void mergePersons(const QList<QUuid> &personIds);

        //! Merge two persons.
        void mergePersons(const QUuid &personId1, const QUuid &personId2);

        //! Find person birth date.
        //! If a birth date is not set it will calculate fuzzy birth date.
        void findPersonBirthAndDeathDates(const QUuid &personId, FDate &birthDate, FDate &deathDate) const;

        //! Import missing persons and relations.
        void importMissing(const FTree *pTree, uint &nImportedPersons, uint &nImportedRelations);

        //! List persons.
        void printPersonsList(const QString &sortPath = QString()) const;

        //! List relations.
        void printRelationsList(const QString &sortPath = QString()) const;

        //! Read from file.
        void readFile(const QString &fileName);

        //! Write to file.
        void writeFile(const QString &fileName) const;

        //! Read from XML file.
        void readXmlFile(const QString &fileName);

        //! Write to XML file.
        void writeXmlFile(const QString &fileName) const;

        //! Read from JSON file.
        void readJsonFile(const QString &fileName);

        //! Write to JSON file.
        void writeJsonFile(const QString &fileName) const;

        //! Read XML element.
        void readXmlElement(QXmlStreamReader &stream, const QString &tag);

        //! Write XML element.
        void writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const;

        //! Create object from Json.
        void fromJson(const QJsonObject &json);

        //! Create Json from object.
        QJsonObject toJson() const;

        //! Write to Dot (Graphviz) file.
        void writeDotFile(const QString &fileName) const;

        //! Compare XML files (produce XML diff).
        static FTreeDiff diffTrees(const FTree *aTree, const FTree *bTree);

        //! Compare XML files (produce XML diff).
        static FTreeDiff diffFiles(const QString &aFileName, const QString &bFileName);

        //! Return json file specs.
        static QByteArray getJsonFileSpecs();

    public:

        //! Generate ID.
        static QUuid generateId();

    signals:

        //! File was loaded.
        void fileLoaded(const QString &fileName);

        //! File was saved.
        void fileSaved(const QString &fileName) const;

        //! Diff between files was produced.
        void diffProduced(const QString &aFileName, const QString &bFileName, const FTreeDiff &diff);

        //! Tree has changed.
        void changed();

        //! Person was added.
        void personAdded(const FPerson &person);

        //! Person was changed.
        void personChanged(const FPerson &person);

        //! Person was removed.
        void personRemoved(QUuid personId);

        //! Relation was added.
        void relationAdded(const FRelation &relation);

        //! Relation was changed.
        void relationChanged(const FRelation &relation);

        //! Relation was removed.
        void relationRemoved(QUuid relationId);

};

#endif /* FL_TREE_H */
