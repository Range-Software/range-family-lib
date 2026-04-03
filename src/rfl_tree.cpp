#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonDocument>

#include "rbl_error.h"
#include "rbl_logger.h"
#include "rfl_tree.h"
#include "rfl_utils.h"

const QString FTree::ElementName::tree("tree");
const QString FTree::ElementName::version("version");
const QString FTree::ElementName::persons("persons");
const QString FTree::ElementName::relations("relations");
const QString FTree::ElementName::person("person");
const QString FTree::ElementName::relation("relation");

const QString FTree::ElementDesc::tree(QT_TRANSLATE_NOOP("FTree","tree"));
const QString FTree::ElementDesc::version(QT_TRANSLATE_NOOP("FTree","version"));
const QString FTree::ElementDesc::persons(QT_TRANSLATE_NOOP("FTree","persons"));
const QString FTree::ElementDesc::relations(QT_TRANSLATE_NOOP("FTree","relations"));
const QString FTree::ElementDesc::person(QT_TRANSLATE_NOOP("FTree","person"));
const QString FTree::ElementDesc::relation(QT_TRANSLATE_NOOP("FTree","relation"));

const RVersion FTree::version(0,0,1);

class FPersonComp
{

    private:

        const QMap<QUuid,FPerson> &persons;
        QString sortPath;

    public:

        FPersonComp(const QMap<QUuid,FPerson> &persons, const QString &sortPath)
            : persons(persons)
            , sortPath(sortPath)
        {
        }

        bool operator()(const QUuid &a,const QUuid &b) const
        {
            return FPerson::compare(this->persons[a],this->persons[b],this->sortPath) < 0;
        }
};

class FRelationComp
{

    private:

        const QMap<QUuid,FRelation> &relations;
        QString sortPath;

    public:

        FRelationComp(const QMap<QUuid,FRelation> &relations, const QString &sortPath)
            : relations(relations)
            , sortPath(sortPath)
        {
        }

        bool operator()(const QUuid &a,const QUuid &b) const
        {
            return FRelation::compare(this->relations[a],this->relations[b],this->sortPath) < 0;
        }
};

FTree::FTree(QObject *parent)
    : QObject{parent}
{
}

void FTree::addPerson(const FPerson &person)
{
    if (person.getId().isNull())
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Null key (person ID: \"%s\").",person.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    if (this->persons.contains(person.getId()))
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Duplicate key (person ID: \"%s\").",person.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    this->persons.insert(person.getId(),person);

    emit this->personAdded(this->persons[person.getId()]);
    emit this->changed();
}

void FTree::setPerson(const FPerson &person)
{
    if (person.getId().isNull())
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Null key (person ID: \"%s\").",person.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    if (!this->persons.contains(person.getId()))
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Non-existing key (person ID: \"%s\").",person.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    this->persons.insert(person.getId(),person);

    emit this->personChanged(this->persons[person.getId()]);
    emit this->changed();
}

void FTree::updatePerson(const FPerson &person)
{
    if (person.getId().isNull())
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Null key (person ID: \"%s\").",person.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    if (!this->persons.contains(person.getId()))
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Non-existing key (person ID: \"%s\").",person.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    FPerson storedPerson = this->findPerson(person.getId());
    storedPerson.update(storedPerson);
    this->persons.insert(storedPerson.getId(),storedPerson);

    emit this->personChanged(this->persons[person.getId()]);
    emit this->changed();
}

void FTree::removePerson(const QUuid &id, bool force)
{
    if (id.isNull())
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Null key (person ID: \"%s\").",id.toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    if (!this->persons.contains(id))
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Non-existing key (person ID: \"%s\").",id.toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    foreach (FRelation relation,this->relations)
    {
        bool relationModified = false;
        if (relation.getPartners().contains(id))
        {
            if (force)
            {
                QList<QUuid> partners = relation.getPartners();
                QMutableListIterator<QUuid> iter(partners);
                while (iter.hasNext())
                {
                    if (iter.next() == id)
                    {
                        iter.remove();
                    }
                }
                relation.setPartners(partners);
                relationModified = true;
            }
            else
            {
                throw RError(RError::Type::InvalidInput,R_ERROR_REF,
                             "Cannot remove key (person ID: \"%s\") because it is listed as a partner in relation (relation ID: \"%s\").",
                             id.toString(QUuid::WithoutBraces).toUtf8().constData(),
                             relation.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
            }
        }
        if (relation.getChildren().contains(id))
        {
            if (force)
            {
                QList<QUuid> children = relation.getChildren();
                QMutableListIterator<QUuid> iter(children);
                while (iter.hasNext())
                {
                    if (iter.next() == id)
                    {
                        iter.remove();
                    }
                }
                relation.setChildren(children);
                relationModified = true;
            }
            else
            {
                throw RError(RError::Type::InvalidInput,R_ERROR_REF,
                             "Cannot remove key (person ID: \"%s\") because it is listed as a child in relation (relation ID: \"%s\").",
                             id.toString(QUuid::WithoutBraces).toUtf8().constData(),
                             relation.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
            }
        }
        if (relationModified)
        {
            this->setRelation(relation);
        }
    }
    this->persons.remove(id);

    emit this->personRemoved(id);
    emit this->changed();
}

FPerson FTree::findPerson(const QUuid &id) const
{
    return this->persons.value(id,FPerson());
}

bool FTree::containsPerson(const QUuid &id) const
{
    return this->persons.contains(id);
}

void FTree::addRelation(const FRelation &relation)
{
    if (relation.getId().isNull())
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Null key (relation ID: \"%s\").",relation.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    if (this->relations.contains(relation.getId()))
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Duplicate key (relation ID: \"%s\").",relation.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    foreach (const QUuid &personId, relation.getPartners())
    {
        if (!this->persons.contains(personId))
        {
            throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Non-existing partner (person ID: \"%s\").",personId.toString(QUuid::WithoutBraces).toUtf8().constData());
        }
    }
    foreach (const QUuid &personId, relation.getChildren())
    {
        if (!this->persons.contains(personId))
        {
            throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Non-existing child (person ID: \"%s\").",personId.toString(QUuid::WithoutBraces).toUtf8().constData());
        }
    }
    this->relations.insert(relation.getId(),relation);

    emit this->relationAdded(this->relations[relation.getId()]);
    emit this->changed();
}

void FTree::setRelation(const FRelation &relation)
{
    if (relation.getId().isNull())
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Null key (relation ID: \"%s\").",relation.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    if (!this->relations.contains(relation.getId()))
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Non-existing key (relation ID: \"%s\").",relation.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    foreach (const QUuid &personId, relation.getPartners())
    {
        if (!this->persons.contains(personId))
        {
            throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Non-existing partner (person ID: \"%s\").",personId.toString(QUuid::WithoutBraces).toUtf8().constData());
        }
    }
    foreach (const QUuid &personId, relation.getChildren())
    {
        if (!this->persons.contains(personId))
        {
            throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Non-existing child (person ID: \"%s\").",personId.toString(QUuid::WithoutBraces).toUtf8().constData());
        }
    }
    this->relations.insert(relation.getId(),relation);

    emit this->relationChanged(this->relations[relation.getId()]);
    emit this->changed();
}

void FTree::updateRelation(const FRelation &relation)
{
    if (relation.getId().isNull())
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Null key (relation ID: \"%s\").",relation.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    if (!this->relations.contains(relation.getId()))
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Non-existing key (relation ID: \"%s\").",relation.getId().toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    foreach (const QUuid &personId, relation.getPartners())
    {
        if (!personId.isNull() && !this->persons.contains(personId))
        {
            throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Non-existing partner (person ID: \"%s\").",personId.toString(QUuid::WithoutBraces).toUtf8().constData());
        }
    }
    foreach (const QUuid &personId, relation.getChildren())
    {
        if (!personId.isNull() && !this->persons.contains(personId))
        {
            throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Non-existing child (person ID: \"%s\").",personId.toString(QUuid::WithoutBraces).toUtf8().constData());
        }
    }
    FRelation storedRelation = this->findRelation(relation.getId());
    storedRelation.update(relation);
    this->relations.insert(storedRelation.getId(),storedRelation);

    emit this->relationChanged(this->relations[storedRelation.getId()]);
    emit this->changed();
}

void FTree::removeRelation(const QUuid &id)
{
    if (id.isNull())
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Null key (relation ID: \"%s\").",id.toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    if (!this->relations.contains(id))
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Non-existing key (relation ID: \"%s\").",id.toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    this->relations.remove(id);

    emit this->relationRemoved(id);
    emit this->changed();
}

FRelation FTree::findRelation(const QUuid &id) const
{
    return this->relations.value(id,FRelation());
}

bool FTree::containsRelation(const QUuid &id) const
{
    return this->relations.contains(id);
}

void FTree::clear()
{
    QList<QUuid> personIds = this->persons.keys();
    QList<QUuid> relationIds = this->relations.keys();

    this->relations.clear();

    foreach (const QUuid &relationId, relationIds)
    {
        emit this->relationRemoved(relationId);
    }

    this->persons.clear();

    foreach (const QUuid &personId, personIds)
    {
        emit this->personRemoved(personId);
    }
    emit this->changed();
}

QList<QUuid> FTree::getPersons(const QString &sortPath) const
{
    QList<QUuid> ids = this->persons.keys();

    if (!sortPath.isEmpty())
    {
        QStringList sortPathList = sortPath.split(QLatin1Char('/'),Qt::SkipEmptyParts);
        if (sortPathList.size() > 0 && sortPathList.at(0) == FTree::ElementName::person)
        {
            sortPathList.pop_front();
            FPersonComp customComp(this->persons,sortPathList.join(QLatin1Char('/')));

            std::sort(ids.begin(),ids.end(),customComp);
        }
    }

    return ids;
}

QList<QUuid> FTree::getRelations(const QString &sortPath) const
{
    QList<QUuid> ids = this->relations.keys();

    if (!sortPath.isEmpty())
    {
        QStringList sortPathList = sortPath.split(QLatin1Char('/'),Qt::SkipEmptyParts);
        if (sortPathList.size() > 0 && sortPathList.at(0) == FTree::ElementName::relation)
        {
            sortPathList.pop_front();
            FRelationComp customComp(this->relations,sortPathList.join(QLatin1Char('/')));

            std::sort(ids.begin(),ids.end(),customComp);
        }
    }

    return ids;
}

QList<QUuid> FTree::findPersonRelations(const QUuid &id) const
{
    QList<QUuid> relationIds = this->relations.keys();
    QList<QUuid> engagedRelationIds;

    foreach (const QUuid &relationId, relationIds)
    {
        const FRelation &relation = this->relations[relationId];
        if (relation.getPartners().contains(id) || relation.getChildren().contains(id))
        {
            engagedRelationIds.append(relationId);
        }
    }

    return engagedRelationIds;
}

QList<QUuid> FTree::findPersonPartnerRelations(const QUuid &id) const
{
    QList<QUuid> relationIds = this->relations.keys();
    QList<QUuid> engagedRelationIds;

    foreach (const QUuid &relationId, relationIds)
    {
        const FRelation &relation = this->relations[relationId];
        if (relation.getPartners().contains(id))
        {
            engagedRelationIds.append(relationId);
        }
    }

    return engagedRelationIds;
}

QList<QUuid> FTree::findPersonChildRelations(const QUuid &id) const
{
    QList<QUuid> relationIds = this->relations.keys();
    QList<QUuid> engagedRelationIds;

    foreach (const QUuid &relationId, relationIds)
    {
        const FRelation &relation = this->relations[relationId];
        if (relation.getChildren().contains(id))
        {
            engagedRelationIds.append(relationId);
        }
    }

    return engagedRelationIds;
}

QList<QUuid> FTree::findRelationPersons(const QUuid &id) const
{
    QList<QUuid> engagedPersonsIds;

    if (this->relations.contains(id))
    {
        foreach (const QUuid &partnerId, this->relations[id].getPartners())
        {
            engagedPersonsIds.append(partnerId);
        }
        foreach (const QUuid &childId, this->relations[id].getChildren())
        {
            engagedPersonsIds.append(childId);
        }
    }

    return engagedPersonsIds;
}

QList<QUuid> FTree::sortPersons(const QList<QUuid> &ids) const
{
    QList<FPerson> persons;
    persons.reserve(ids.size());

    foreach (const QUuid &id,ids)
    {
        persons.append(this->findPerson(id));
    }
    std::sort(persons.begin(),persons.end(),
              [](const FPerson &a, const FPerson &b) { return a.getBirth().getDate().toDate() < b.getBirth().getDate().toDate(); });

    QList<QUuid> sortedIds;
    sortedIds.reserve(persons.size());
    foreach (const FPerson &person, persons)
    {
        sortedIds.append(person.getId());
    }

    return sortedIds;
}

void FTree::mergePersons(const QList<QUuid> &personIds)
{

    if (personIds.size() < 2)
    {
        return;
    }

    bool isFirst = true;
    foreach (QUuid personId, personIds)
    {
        if (isFirst)
        {
            isFirst = false;
            continue;
        }

        this->mergePersons(personIds.at(0),personId);
    }
}

void FTree::mergePersons(const QUuid &personId1, const QUuid &personId2)
{
    FPerson person1 = this->findPerson(personId1);
    FPerson person2 = this->findPerson(personId2);

    RLogger::info("Merging person \"%s\" with  \"%s\"\n",person1.getName().toString().toUtf8().constData(),person2.getName().toString().toUtf8().constData());

    person1.merge(person2);
    this->setPerson(person1);

    for (auto iter = this->relations.cbegin(); iter != this->relations.cend(); ++iter)
    {
        FRelation &relation = this->relations[iter.key()];

        bool relationChanged = false;

        QList<QUuid> &partnerIds = relation.getPartners();
        for (int i=0;i<partnerIds.size();i++)
        {
            if (partnerIds[i] == personId2)
            {
                partnerIds[i] = personId1;
                relationChanged = true;
            }
        }

        QList<QUuid> &childrenIds = relation.getChildren();
        for (int i=0;i<childrenIds.size();i++)
        {
            if (childrenIds[i] == personId2)
            {
                childrenIds[i] = personId1;
                relationChanged = true;
            }
        }

        if (relationChanged)
        {
            emit this->relationChanged(relation);
        }
    }

    this->removePerson(personId2,false);
}

void FTree::findPersonBirthAndDeathDates(const QUuid &personId, FDate &birthDate, FDate &deathDate) const
{
    FPerson person = this->findPerson(personId);

    birthDate.empty(true);
    deathDate.empty(true);

    bool birthDateSet = false;
    bool deathDateSet = false;

    if (!person.getBirth().getDate().getFuzzy() && !person.getBirth().getDate().isEmpty())
    {
        birthDate = person.getBirth().getDate();
        birthDateSet = true;
    }
    if (birthDateSet && !person.getDeath().getDate().isEmpty())
    {
        deathDate = person.getDeath().getDate();
        deathDateSet = true;
    }

    if (birthDateSet && deathDateSet)
    {
        return;
    }

    FDate lowBirthDate(birthDate), highBirthDate(birthDate);
    FDate lowDeathDate(deathDate), highDeathDate(deathDate);

    // Estimate birth date range based on death date if possible.
    if (!birthDateSet && !person.getDeath().getDate().isEmpty() && person.getDeath().getDate().isValid())
    {
        // Person should not be born befor his/her own death -max. age
        lowBirthDate = FDate(person.getDeath().getDate().toDate().addYears(-FConstants::maxAge));
        // Person can not be born after his/her own death
        highBirthDate = person.getDeath().getDate();
    }

    // Estimate death date range based on birth date if possible.
    if (!deathDateSet && !person.getBirth().getDate().isEmpty() && person.getBirth().getDate().isValid())
    {
        // Person can not be dead before his/her birth date
        lowDeathDate = person.getBirth().getDate();
        // Person should not be dead after his/her birth date +max. age
        highDeathDate = FDate(person.getBirth().getDate().toDate().addYears(FConstants::maxAge));
    }

//    bool isParent = false;

    // Check person's children.
    foreach (auto relationId, this->findPersonPartnerRelations(personId))
    {
        const FRelation &relation = this->relations[relationId];

        // Check relation
        if (!relation.getBegin().getDate().isEmpty())
        {
            if (!birthDateSet)
            {
                // Person should not be born before relation start date -max. reproductive age
                FDate possibleLowBirthDate(relation.getBegin().getDate().toDate().addYears(-FConstants::maxReproductiveAge));
                if (lowBirthDate.isEmpty() || lowBirthDate.toDate() < possibleLowBirthDate.toDate())
                {
                    lowBirthDate = possibleLowBirthDate;
                }
                // Person should not be born after relation start date -min. reproductive age
                FDate possibleHighBirthDate(relation.getBegin().getDate().toDate().addYears(-FConstants::minReproductiveAge));
                if (highBirthDate.isEmpty() || highBirthDate.toDate() > possibleHighBirthDate.toDate())
                {
                    highBirthDate = possibleHighBirthDate;
                }
            }
            if (!deathDateSet)
            {
                // Person should not be dead before relation start date
                FDate possibleLowDeathDate(relation.getBegin().getDate());
                if (lowDeathDate.isEmpty() || lowDeathDate.toDate() < possibleLowDeathDate.toDate())
                {
                    lowDeathDate = possibleLowDeathDate;
                }
            }
        }

        if (!deathDateSet)
        {
            if (!relation.getEnd().getDate().isEmpty())
            {
                // Person should not be dead before relation end date
                FDate possibleLowDeathDate(relation.getEnd().getDate());
                if (lowDeathDate.isEmpty() || lowDeathDate.toDate() < possibleLowDeathDate.toDate())
                {
                    lowDeathDate = possibleLowDeathDate;
                }
            }
        }

        // Check children directly
        foreach (auto childId, relation.getChildren())
        {
//            isParent = true;
            FPerson child = this->findPerson(childId);
            if (!child.getBirth().getDate().isEmpty())
            {
                if (!birthDateSet)
                {
                    // Person should not be born before child's birth date -max. reproductive age
                    FDate possibleLowBirthDate(child.getBirth().getDate().toDate().addYears(-FConstants::maxReproductiveAge));
                    if (lowBirthDate.isEmpty() || lowBirthDate.toDate() < possibleLowBirthDate.toDate())
                    {
                        lowBirthDate = possibleLowBirthDate;
                    }
                    // Person should not be born after child's birth date -min. reproductive age
                    FDate possibleHighBirthDate(child.getBirth().getDate().toDate().addYears(-FConstants::minReproductiveAge));
                    if (highBirthDate.isEmpty() || highBirthDate.toDate() > possibleHighBirthDate.toDate())
                    {
                        highBirthDate = possibleHighBirthDate;
                    }
                }
                if (!deathDateSet)
                {
                    // Person should not be dead before child birth date
                    FDate possibleLowDeathDate(child.getBirth().getDate());
                    if (lowDeathDate.isEmpty() || lowDeathDate.toDate() < possibleLowDeathDate.toDate())
                    {
                        lowDeathDate = possibleLowDeathDate;
                    }
                }
            }
        }
    }

    if (lowDeathDate.isEmpty())
    {
        lowDeathDate = highBirthDate;
    }

    if (highDeathDate.isEmpty() || lowDeathDate.toDate() > highDeathDate.toDate())
    {
        highDeathDate = lowDeathDate;
    }

    // Check person's parents.
    foreach (auto relationId, this->findPersonChildRelations(personId))
    {
        const FRelation &relation = this->relations[relationId];

        if (!birthDateSet)
        {
            // Check relation
            if (!relation.getBegin().getDate().isEmpty())
            {
                // Person should not be born before parents relation has started
                FDate possibleLowDate(relation.getBegin().getDate());
                if (lowBirthDate.isEmpty() || lowBirthDate.toDate() < possibleLowDate.toDate())
                {
                    lowBirthDate = possibleLowDate;
                }
            }
            if (!relation.getEnd().getDate().isEmpty())
            {
                // Person should not be born after parents relation has ended
                FDate possibleHighDate(relation.getEnd().getDate());
                if (highBirthDate.isEmpty() || highBirthDate.toDate() > possibleHighDate.toDate())
                {
                    highBirthDate = possibleHighDate;
                }
            }

            // Check parents directly
            foreach (auto partnerId, relation.getPartners())
            {
                FPerson partner = this->findPerson(partnerId);
                if (!partner.getBirth().getDate().isEmpty())
                {
                    if (!birthDateSet)
                    {
                        // Person should not be born before parent's birth date + min. reproductive age
                        FDate possibleLowDate(partner.getBirth().getDate().toDate().addYears(FConstants::minReproductiveAge));
                        if (lowBirthDate.isEmpty() || lowBirthDate.toDate() < possibleLowDate.toDate())
                        {
                            lowBirthDate = possibleLowDate;
                        }
                        // Person should not be born after parent's birth date + max. reproductive age
                        FDate possibleHighDate(partner.getBirth().getDate().toDate().addYears(FConstants::maxReproductiveAge));
                        if (highBirthDate.isEmpty() || highBirthDate.toDate() > possibleHighDate.toDate())
                        {
                            highBirthDate = possibleHighDate;
                        }
                    }
                }
            }
        }

        // Check siblings
        foreach (auto siblingId, relation.getChildren())
        {
            FPerson sibling = this->findPerson(siblingId);
            if (!sibling.getBirth().getDate().isEmpty())
            {
                if (!birthDateSet)
                {
                    // Person should not be born before sibling's birth date -max. reproductive age
                    FDate possibleLowDate(sibling.getBirth().getDate().toDate().addYears(-FConstants::maxReproductiveAge));
                    if (lowBirthDate.isEmpty() || lowBirthDate.toDate() < possibleLowDate.toDate())
                    {
                        lowBirthDate = possibleLowDate;
                    }
                }
            }
        }
    }

//    if (isParent)
//    {
//        if (lowBirthDate.toDate().addDays(FConstants::maxReproductiveAge))
//    }

    if (!birthDateSet)
    {
        birthDate = FDate::findMiddle(lowBirthDate,highBirthDate);
        birthDate.setFuzzy(true);
    }
    if (!deathDateSet)
    {
        // Calculate maximum high death date to minimize possibility of predicting death of still alive person.
        if (birthDateSet)
        {
            if (highDeathDate.toDate() > birthDate.toDate().addYears(FConstants::maxAge))
            {
                highDeathDate = birthDate.toDate().addYears(FConstants::maxAge);
            }
        }
        else
        {
            highDeathDate = highBirthDate.toDate().addYears(FConstants::maxAge);
        }

        if (highDeathDate.toDate() < QDate::currentDate())
        {
            deathDate = FDate::findMiddle(lowDeathDate,highDeathDate);
            deathDate.setFuzzy(true);
        }
    }
}

void FTree::importMissing(const FTree *pTree, uint &nImportedPersons, uint &nImportedRelations)
{
    nImportedPersons = 0;
    nImportedRelations = 0;

    for (auto iter = pTree->persons.cbegin(); iter != pTree->persons.cend(); ++iter)
    {
        if (!this->containsPerson(iter.value().getId()))
        {
            this->addPerson(iter.value());
            nImportedPersons++;
        }
    }

    for (auto iter = this->relations.cbegin(); iter != this->relations.cend(); ++iter)
    {
        if (!this->containsRelation(iter.value().getId()))
        {
            this->addRelation(iter.value());
            nImportedRelations++;
        }
    }
}

void FTree::printPersonsList(const QString &sortPath) const
{
    QList<QUuid> ids = this->getPersons(sortPath);

    foreach (const QUuid &id, ids)
    {
        const FPerson &person = this->persons[id];
        RLogger::info("%s - %s (%s / %s)\n",
                      person.getId().toString(QUuid::WithoutBraces).toUtf8().constData(),
                      person.getName().toString().toUtf8().constData(),
                      person.getBirth().getDate().toString().toUtf8().constData(),
                      person.getDeath().getDate().toString().toUtf8().constData());
    }
}

void FTree::printRelationsList(const QString &sortPath) const
{
    QList<QUuid> ids = this->getRelations(sortPath);

    foreach (const QUuid &id, ids)
    {
        const FRelation &relation = this->relations[id];
        QString partnerNames;

        foreach (const QUuid &partnerId,relation.getPartners())
        {
            if (!partnerNames.isEmpty())
            {
                partnerNames +=" + ";
            }
            if (this->persons.contains(partnerId))
            {
                partnerNames += this->persons[partnerId].getName().toString();
            }
            else
            {
                partnerNames += partnerId.toString(QUuid::WithoutBraces);
            }
        }

        RLogger::info("%s - %s (%s / %s)\n",relation.getId().toString(QUuid::WithoutBraces).toUtf8().constData(),
                      partnerNames.toUtf8().constData(),
                      relation.getBegin().getDate().toString().toUtf8().constData(),
                      relation.getEnd().getDate().toString().toUtf8().constData());
    }
}

void FTree::readFile(const QString &fileName)
{
    if (fileName.isEmpty())
    {
        throw RError(RError::Type::InvalidFileName,R_ERROR_REF,"No file name was provided.");
    }

    QFileInfo fi(fileName);
    QString suffix = fi.suffix().toUpper();

    if (suffix == "XML")
    {
        this->readXmlFile(fileName);
    }
    else if (suffix == "JSON")
    {
        this->readJsonFile(fileName);
    }
    else
    {
        throw RError(RError::Type::InvalidFileName,R_ERROR_REF,"Unsupported file type \"%s\".",suffix.toUtf8().constData());
    }
}

void FTree::writeFile(const QString &fileName) const
{
    if (fileName.isEmpty())
    {
        throw RError(RError::Type::InvalidFileName,R_ERROR_REF,"No file name was provided.");
    }

    QFileInfo fi(fileName);
    QString suffix = fi.suffix().toUpper();

    if (suffix == "XML")
    {
        this->writeXmlFile(fileName);
    }
    else if (suffix == "JSON")
    {
        this->writeJsonFile(fileName);
    }
    else
    {
        throw RError(RError::Type::InvalidFileName,R_ERROR_REF,"Unsupported file type \"%s\".",suffix.toUtf8().constData());
    }
}

void FTree::readXmlFile(const QString &fileName)
{
    if (fileName.isEmpty())
    {
        throw RError(RError::Type::InvalidFileName,R_ERROR_REF,"No file name was provided.");
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw RError(RError::Type::OpenFile,R_ERROR_REF,"Failed to open the file \"%s\".",fileName.toUtf8().constData());
    }

    RLogger::info("Reading XML file \"%s\"\n",fileName.toUtf8().constData());
    RLogger::indent();

    QXmlStreamReader stream(&file);

    this->readXmlElement(stream,FTree::ElementName::tree);

    if (stream.hasError())
    {
        throw RError(RError::Type::ReadFile,R_ERROR_REF,"Failed to read the file \"%s\" line='%u' column='%u' offset='%u'. '%s'",
                     fileName.toUtf8().constData(),
                     stream.lineNumber(),
                     stream.columnNumber(),
                     stream.characterOffset(),
                     stream.errorString().toUtf8().constData());
    }

    file.close();

    RLogger::unindent();

    emit this->fileLoaded(fileName);
}

void FTree::writeXmlFile(const QString &fileName) const
{
    if (fileName.isEmpty())
    {
        throw RError(RError::Type::InvalidFileName,R_ERROR_REF,"No file name was provided.");
    }

    QSaveFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        throw RError(RError::Type::OpenFile,R_ERROR_REF,"Failed to open the file \"%s\".",fileName.toUtf8().constData());
    }

    RLogger::info("Writing XML file \"%s\"\n",fileName.toUtf8().constData());
    RLogger::indent();

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    this->writeXmlElement(stream,FTree::ElementName::tree);

    stream.writeEndDocument();

    file.commit();

    RLogger::unindent();

    emit this->fileSaved(fileName);
}

void FTree::readJsonFile(const QString &fileName)
{
    if (fileName.isEmpty())
    {
        throw RError(RError::Type::InvalidFileName,R_ERROR_REF,"No file name was provided.");
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw RError(RError::Type::OpenFile,R_ERROR_REF,"Failed to open the file \"%s\".",fileName.toUtf8().constData());
    }

    RLogger::info("Reading JSON file \"%s\"\n",fileName.toUtf8().constData());
    RLogger::indent();

    QByteArray byteArray = file.readAll();
    RLogger::info("Successfuly read \"%ld\" bytes from \"%s\".\n",byteArray.size(),file.fileName().toUtf8().constData());

    this->fromJson(QJsonDocument::fromJson(byteArray).object());

    file.close();

    RLogger::unindent();

    emit this->fileLoaded(fileName);
}

void FTree::writeJsonFile(const QString &fileName) const
{
    if (fileName.isEmpty())
    {
        throw RError(RError::Type::InvalidFileName,R_ERROR_REF,"No file name was provided.");
    }

    QSaveFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        throw RError(RError::Type::OpenFile,R_ERROR_REF,"Failed to open the file \"%s\".",fileName.toUtf8().constData());
    }

    RLogger::info("Writing JSON file \"%s\"\n",fileName.toUtf8().constData());
    RLogger::indent();

    qint64 bytesOut = file.write(QJsonDocument(this->toJson()).toJson());

    RLogger::info("Successfuly wrote \"%ld\" bytes to \"%s\".\n",bytesOut,file.fileName().toUtf8().constData());

    file.commit();

    RLogger::unindent();

    emit this->fileSaved(fileName);
}

void FTree::readXmlElement(QXmlStreamReader &stream, const QString &tag)
{
    this->clear();

    if (stream.readNextStartElement())
    {
        if (stream.qualifiedName().toString() != tag)
        {
            stream.raiseError(QObject::tr("Missing element") + " <" + tag + ">.");
        }

        if (!stream.attributes().hasAttribute(FTree::ElementName::version))
        {
            stream.raiseError(QObject::tr("Missing attribute") + " \'" + FTree::ElementName::version + "\' in element <" + tag + ">.");
        }
        RVersion fileVersion(stream.attributes().value(FTree::ElementName::version).toString());
        if (fileVersion.getMajor() > FTree::version.getMajor())
        {
            stream.raiseError(QObject::tr("File version is not supported") + ": " + fileVersion.toString() + ".");
        }
        while(stream.readNextStartElement())
        {
            if (stream.qualifiedName().toString() == FTree::ElementName::persons)
            {
                while(stream.readNextStartElement())
                {
                    if (stream.qualifiedName().toString() == FTree::ElementName::person)
                    {
                        FPerson person;
                        person.readXmlElement(stream,FTree::ElementName::person);
                        this->addPerson(person);
                    }
                    else
                    {
                        stream.raiseError(QObject::tr("Unexpected element") + " <" + stream.qualifiedName().toString() + ">.");
                    }
                }
            }
            else if (stream.qualifiedName().toString() == FTree::ElementName::relations)
            {
                while(stream.readNextStartElement())
                {
                    if (stream.qualifiedName().toString() == FTree::ElementName::relation)
                    {
                        FRelation relation;
                        relation.readXmlElement(stream,FTree::ElementName::relation);
                        this->addRelation(relation);
                    }
                    else
                    {
                        stream.raiseError(QObject::tr("Unexpected element") + " <" + stream.qualifiedName().toString() + ">.");
                    }
                }
            }
            else
            {
                stream.raiseError(QObject::tr("Unexpected element") + " <" + stream.qualifiedName().toString() + ">.");
            }
        }
    }
}

void FTree::writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const
{
    stream.writeStartElement(tag);
    stream.writeAttribute(FTree::ElementName::version,FTree::version.toString());

    // Write list of persons
    stream.writeStartElement(FTree::ElementName::persons);

    foreach (const FPerson &person, this->persons)
    {
        person.writeXmlElement(stream,FTree::ElementName::person);
    }

    stream.writeEndElement(); // persons

    // Write list of relations
    stream.writeStartElement(FTree::ElementName::relations);

    foreach (const FRelation &relation, this->relations)
    {
        relation.writeXmlElement(stream,FTree::ElementName::relation);
    }

    stream.writeEndElement(); // relations

    stream.writeEndElement(); // tree
}

void FTree::fromJson(const QJsonObject &json)
{
    this->clear();

    if (const QJsonValue &v = json[FTree::ElementName::version]; v.isString())
    {
        RVersion fileVersion(v.toString());
        if (fileVersion.getMajor() > FTree::version.getMajor())
        {
            RLogger::error("File version is not supported: %s\n",fileVersion.toString().toUtf8().constData());
        }
    }

    if (const QJsonValue &v = json[FTree::ElementName::persons]; v.isArray())
    {
        const QJsonArray &jsonArray = v.toArray();

        for (const QJsonValue &jsonValue : jsonArray)
        {
            if (jsonValue.isObject())
            {
                this->addPerson(FPerson::fromJson(jsonValue.toObject()));
            }
        }
    }

    if (const QJsonValue &v = json[FTree::ElementName::relations]; v.isArray())
    {
        const QJsonArray &jsonArray = v.toArray();

        for (const QJsonValue &jsonValue : jsonArray)
        {
            if (jsonValue.isObject())
            {
                this->addRelation(FRelation::fromJson(jsonValue.toObject()));
            }
        }
    }
}

QJsonObject FTree::toJson() const
{
    QJsonObject json;

    json[FTree::ElementName::version] = this->version.toString();

    QJsonArray personsArray;
    for (const FPerson &person : this->persons)
    {
        personsArray.append(person.toJson());
    }
    json[FTree::ElementName::persons] = personsArray;

    QJsonArray relationsArray;
    for (const FRelation &relation : this->relations)
    {
        relationsArray.append(relation.toJson());
    }
    json[FTree::ElementName::relations] = relationsArray;

    return json;
}

void FTree::writeDotFile(const QString &fileName) const
{
    if (fileName.isEmpty())
    {
        throw RError(RError::Type::InvalidFileName,R_ERROR_REF,"No file name was provided.");
    }

    QSaveFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        throw RError(RError::Type::OpenFile,R_ERROR_REF,"Failed to open the file \"%s\".",fileName.toUtf8().constData());
    }

    QTextStream stream(&file);

    stream << "strict digraph FamilyTree {" << Qt::endl;
    stream << "  graph [overlap=false,splines=true];" << Qt::endl;

    foreach (const FPerson &person,this->persons)
    {
        QString color = "gray";
        if (person.getSex() == "male")
        {
            color = "blue";
        }
        else if (person.getSex() == "female")
        {
            color = "red";
        }
        QString period = person.getBirth().getDate().toString() + " - " + person.getDeath().getDate().toString();
        stream << "  \"" << person.getId().toString(QUuid::WithoutBraces)
               << "\" [shape=box,color=" << color << ",label=\"" << person.getName().toString() << "\n" << period <<  "\"]" << Qt::endl;
    }

    foreach (const FRelation &relation,this->relations)
    {
        QString period = relation.getBegin().getDate().toString() + " - " + relation.getEnd().getDate().toString();
        stream << "  \"" << relation.getId().toString(QUuid::WithoutBraces)
               << "\" [shape=ellipse,color=green,label=\"" << period << "\"]" << Qt::endl;

        foreach (const QUuid &partnerId,relation.getPartners())
        {
            stream << "  \"" << relation.getId().toString(QUuid::WithoutBraces)
                   << "\" -> \"" << partnerId.toString(QUuid::WithoutBraces) << "\" [dir=back]" << Qt::endl;
        }
        foreach (const QUuid &childId,relation.getChildren())
        {
            stream << "  \"" << relation.getId().toString(QUuid::WithoutBraces)
                   << "\" -> \"" << childId.toString(QUuid::WithoutBraces) << "\"" << Qt::endl;
        }
    }

    stream << "}" << Qt::endl;

    file.commit();
}

FTreeDiff FTree::diffTrees(const FTree *aTree, const FTree *bTree)
{
    FTreeDiff treeDiff;

    QSet<QUuid> aIds;
    QSet<QUuid> bIds;

    // Compare persons.

    RLogger::info("Comparing persons\n");
    RLogger::indent();

    FTreeDiff::Set &personsDiff = treeDiff.getPersons();

    aIds.clear();
    bIds.clear();

    aIds.reserve(aTree->persons.size());
    bIds.reserve(bTree->persons.size());

    for (auto aIter = aTree->persons.cbegin(), end = aTree->persons.cend(); aIter != end; ++aIter)
    {
        aIds.insert(aIter.key());
    }
    for (auto bIter = bTree->persons.cbegin(), end = bTree->persons.cend(); bIter != end; ++bIter)
    {
        bIds.insert(bIter.key());
    }

    for (const QUuid &id : aIds)
    {
        QSet<QUuid>::const_iterator iter = bIds.constFind(id);
        if (iter == bIds.cend())
        {
            personsDiff.a.insert(id);
        }
        else
        {
            FPerson aPerson = aTree->findPerson(*iter);
            FPerson bPerson = bTree->findPerson(*iter);

            if (aPerson != bPerson)
            {
                personsDiff.ab.insert(id);
            }
        }
    }

    RLogger::info("Found %ld persons only in a-tree\n",personsDiff.a.size());
    RLogger::info("Found %ld persons only in b-tree\n",personsDiff.b.size());
    RLogger::info("Found %ld persons which are different in both trees\n",personsDiff.ab.size());
    RLogger::unindent();

    for (const QUuid &id : bIds)
    {
        QSet<QUuid>::const_iterator iter = aIds.constFind(id);
        if (iter == aIds.cend())
        {
            personsDiff.b.insert(id);
        }
    }

    // Compare relations.

    RLogger::info("Comparing relations\n");
    RLogger::indent();

    FTreeDiff::Set &relationsDiff = treeDiff.getRelations();

    aIds.clear();
    bIds.clear();

    aIds.reserve(aTree->relations.size());
    bIds.reserve(bTree->relations.size());

    for (auto aIter = aTree->relations.cbegin(), end = aTree->relations.cend(); aIter != end; ++aIter)
    {
        aIds.insert(aIter.key());
    }
    for (auto bIter = bTree->relations.cbegin(), end = bTree->relations.cend(); bIter != end; ++bIter)
    {
        bIds.insert(bIter.key());
    }

    for (const QUuid &id : aIds)
    {
        QSet<QUuid>::const_iterator iter = bIds.constFind(id);
        if (iter == bIds.cend())
        {
            relationsDiff.a.insert(id);
        }
        else
        {
            FRelation aRelation = aTree->findRelation(*iter);
            FRelation bRelation = bTree->findRelation(*iter);

            if (aRelation != bRelation)
            {
                relationsDiff.ab.insert(id);
            }
        }
    }

    for (const QUuid &id : bIds)
    {
        QSet<QUuid>::const_iterator iter = aIds.constFind(id);
        if (iter == aIds.cend())
        {
            relationsDiff.b.insert(id);
        }
    }

    RLogger::info("Found %ld relations only in a-tree\n",relationsDiff.a.size());
    RLogger::info("Found %ld relations only in b-tree\n",relationsDiff.b.size());
    RLogger::info("Found %ld relations which are different in both trees\n",relationsDiff.ab.size());
    RLogger::unindent(true);

    return treeDiff;
}

FTreeDiff FTree::diffFiles(const QString &aFileName, const QString &bFileName)
{
    FTree a, b;

    RLogger::info("Reading a-File \"%s\" file to compare\n",aFileName.toUtf8().constData());
    a.readFile(aFileName);
    RLogger::info("Reading b-File \"%s\" file to compare\n",bFileName.toUtf8().constData());
    b.readFile(bFileName);

    return FTree::diffTrees(&a,&b);
}

QByteArray FTree::getJsonFileSpecs()
{
    QByteArray jsonFileSpecs = R"(FAMILY TREE JSON FILE SPECIFICATION

TOP-LEVEL STRUCTURE
The JSON file has three top-level keys:
- "persons": array of Person objects
- "relations": array of Relation objects
- "version": string (e.g. "0.0.1")

PERSON OBJECT
Required fields:
- "id": string, UUID v4, unique identifier
- "name": Name object (see below)
- "sex": string, "male" or "female"

Optional fields:
- "birth": Event object describing birth
- "death": Event object describing death
- "picture": Picture object with base64-encoded PNG photo
- "text": string, base64-encoded notes or biography

NAME OBJECT
Required:
- "first-name": string
- "last-name": string
Optional:
- "middle-name": string
- "maiden-name": string, birth surname before marriage

EVENT OBJECT (used for birth, death, relation begin/end)
Optional fields:
- "date": Date object
- "place": Place object

DATE OBJECT
All fields required when date is present:
- "year": integer
- "month": integer (1-12)
- "day": integer (1-31)
- "fuzzy": boolean, true if date is approximate or uncertain

PLACE OBJECT
Optional:
- "address": Address object

ADDRESS OBJECT
All fields optional:
- "city": string
- "country": string
- "street-number": string

PICTURE OBJECT
Required:
- "base64Data": string, base64-encoded PNG image

RELATION OBJECT
A relation represents a partnership (marriage or similar) and its children.
Required:
- "id": string, UUID v4, unique identifier
Optional:
- "partners": array of person UUIDs, typically 2 (supports 0, 1, or 2 for incomplete records)
- "children": array of person UUIDs
- "begin": Event object (e.g. marriage date/place)
- "end": Event object (e.g. divorce date/place)

RULES AND CONSTRAINTS
- All IDs use UUID v4 format.
- All UUIDs in "partners" and "children" must reference valid person IDs in the "persons" array.
- File encoding is UTF-8.
- "text" and "picture.base64Data" fields are base64-encoded.
- Omitted optional fields mean the information is unknown, not that it doesn't exist.

MINIMAL VALID EXAMPLE
{
  "version": "0.0.1",
  "persons": [
    {
      "id": "a1b2c3d4-e5f6-7890-abcd-ef1234567890",
      "name": { "first-name": "John", "last-name": "Smith" },
      "sex": "male",
      "birth": {
        "date": { "day": 15, "month": 6, "year": 1980, "fuzzy": false },
        "place": { "address": { "city": "Prague", "country": "Czech Republic" } }
      }
    },
    {
      "id": "e5f6g7h8-i9j0-1234-klmn-op5678901234",
      "name": { "first-name": "Jane", "last-name": "Smith", "maiden-name": "Doe" },
      "sex": "female"
    }
  ],
  "relations": [
    {
      "id": "r1s2t3u4-v5w6-7890-xyza-bc1234567890",
      "partners": ["a1b2c3d4-e5f6-7890-abcd-ef1234567890", "e5f6g7h8-i9j0-1234-klmn-op5678901234"],
      "children": [],
      "begin": { "date": { "day": 20, "month": 8, "year": 2010, "fuzzy": false } }
    }
  ]
}
)";
    return jsonFileSpecs;
}

QUuid FTree::generateId()
{
    return QUuid::createUuid();
}

