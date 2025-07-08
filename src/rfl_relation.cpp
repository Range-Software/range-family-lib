#include <QJsonArray>

#include <rbl_error.h>
#include <rbl_logger.h>

#include "rfl_relation.h"

const QString FRelation::ElementName::id("id");
const QString FRelation::ElementName::partners("partners");
const QString FRelation::ElementName::children("children");
const QString FRelation::ElementName::partnerId("partner-id");
const QString FRelation::ElementName::childId("child-id");
const QString FRelation::ElementName::begin("begin");
const QString FRelation::ElementName::end("end");

const QString FRelation::ElementDesc::id(QT_TRANSLATE_NOOP("FRelation","ID"));
const QString FRelation::ElementDesc::partners(QT_TRANSLATE_NOOP("FRelation","Partners"));
const QString FRelation::ElementDesc::children(QT_TRANSLATE_NOOP("FRelation","Children"));
const QString FRelation::ElementDesc::partnerId(QT_TRANSLATE_NOOP("FRelation","Partner ID"));
const QString FRelation::ElementDesc::childId(QT_TRANSLATE_NOOP("FRelation","Child ID"));
const QString FRelation::ElementDesc::begin(QT_TRANSLATE_NOOP("FRelation","Begin"));
const QString FRelation::ElementDesc::end(QT_TRANSLATE_NOOP("FRelation","End"));

void FRelation::_init(const FRelation *pRelation)
{
    if (pRelation)
    {
        this->id = pRelation->id;
        this->partners = pRelation->partners;
        this->children = pRelation->children;
        this->begin = pRelation->begin;
        this->end = pRelation->end;
    }
}

FRelation::FRelation()
{
    this->_init ();
}

FRelation::FRelation(const FRelation &relation)
{
    this->_init (&relation);
}

FRelation::~FRelation()
{
}

FRelation & FRelation::operator =(const FRelation &relation)
{
    this->_init(&relation);
    return (*this);
}

bool FRelation::operator ==(const FRelation &relation) const
{
    return (this->id == relation.id &&
            FRelation::compareListContents(this->partners,relation.partners) &&
            FRelation::compareListContents(this->children,relation.children) &&
            this->begin == relation.begin &&
            this->end == relation.end);
}

bool FRelation::operator !=(const FRelation &relation) const
{
    return !((*this) == relation);
}

void FRelation::update(const FRelation &relation)
{
    if (relation.partners.size() >= this->partners.size())
    {
        if (relation.partners[0].isNull())
        {
            partners.resize(0);
        }
        else
        {
            this->partners = relation.partners;
        }
    }
    if (relation.children.size() >= this->children.size())
    {
        if (relation.children[0].isNull())
        {
            children.resize(0);
        }
        else
        {
            this->children = relation.children;
        }
    }
    this->begin.update(relation.begin);
    this->end.update(relation.end);
}

bool FRelation::isEmpty() const
{
    return this->id.isNull() && this->begin.isEmpty() && this->end.isEmpty();
}

bool FRelation::isValid() const
{
    int nPartners = this->partners.size();
    for (int i=0;i<nPartners;i++)
    {
        for (int j=i+1;j<nPartners;j++)
        {
            if (this->partners[i] == this->partners[j])
            {
                return false;
            }
        }
        if (this->children.contains(this->partners[i]))
        {
            return false;
        }
    }
    int nChildren = this->children.size();
    for (int i=0;i<nChildren;i++)
    {
        for (int j=i+1;j<nChildren;j++)
        {
            if (this->children[i] == this->children[j])
            {
                return false;
            }
        }
    }

    return begin.isValid() && end.isValid();
}

void FRelation::validate() const
{
    int nPartners = this->partners.size();
    for (int i=0;i<nPartners;i++)
    {
        for (int j=i+1;j<nPartners;j++)
        {
            if (this->partners[i] == this->partners[j])
            {
                throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Partner list is not valid (duplicate partner: \'%s\').",this->partners[i].toString(QUuid::WithoutBraces).toUtf8().constData());
            }
        }
        if (this->children.contains(this->partners[i]))
        {
            throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Duplicate role (person ID: \'%s\'). Person cannot be both partner and child in one relation.",this->partners[i].toString(QUuid::WithoutBraces).toUtf8().constData());
        }
    }
    int nChildren = this->children.size();
    for (int i=0;i<nChildren;i++)
    {
        for (int j=i+1;j<nChildren;j++)
        {
            if (this->children[i] == this->children[j])
            {
                throw RError(RError::Type::InvalidInput,R_ERROR_REF,"iChildren list is not valid (duplicate child: \'%s\').",this->partners[i].toString(QUuid::WithoutBraces).toUtf8().constData());
            }
        }
    }
    begin.validate();
    end.validate();
}

const QUuid &FRelation::getId() const
{
    return this->id;
}

void FRelation::setId(const QUuid &id)
{
    this->id = id;
}

const QList<QUuid> &FRelation::getPartners() const
{
    return this->partners;
}

QList<QUuid> &FRelation::getPartners()
{
    return this->partners;
}

void FRelation::setPartners(const QList<QUuid> &partners)
{
    this->partners = partners;
}

const QList<QUuid> &FRelation::getChildren() const
{
    return this->children;
}

QList<QUuid> &FRelation::getChildren()
{
    return this->children;
}

void FRelation::setChildren(const QList<QUuid> &children)
{
    this->children = children;
}

const FEvent &FRelation::getBegin() const
{
    return this->begin;
}

void FRelation::setBegin(const FEvent &begin)
{
    this->begin = begin;
}

const FEvent &FRelation::getEnd() const
{
    return this->end;
}

void FRelation::setEnd(const FEvent &end)
{
    this->end = end;
}

QString FRelation::getPartnersString() const
{
    QString partnersId;
    foreach (const QUuid &partnerId, this->partners)
    {
        if (!partnersId.isEmpty())
        {
            partnersId += ",";
        }
        partnersId += partnerId.toString(QUuid::WithoutBraces);
    }
    return partnersId;
}

QString FRelation::getChildrenString() const
{
    QString childrenId;

    foreach (const QUuid &childId, this->children)
    {
        if (!childrenId.isEmpty())
        {
            childrenId += ",";
        }
        childrenId += childId.toString(QUuid::WithoutBraces);
    }

    return childrenId;
}

void FRelation::merge(const FRelation &relation)
{
    for (const QUuid &id : relation.partners)
    {
        if (!this->partners.contains(id))
        {
            this->partners.append(id);
        }
    }
    for (const QUuid &id : relation.children)
    {
        if (!this->children.contains(id))
        {
            this->children.append(id);
        }
    }
    this->begin.merge(relation.begin);
    this->end.merge(relation.end);
}

QString FRelation::toString() const
{

    return this->getPartnersString() + "/"
         + this->getChildrenString() + "/"
         + this->begin.toString() + "/"
         + this->end.toString();
}

QString FRelation::toShortString() const
{
    QString shortString;

    shortString = QString("%1: %2, %3: %4").arg(QT_TRANSLATE_NOOP("FRelation","Partners"),QString::number(this->partners.size()),
                                                QT_TRANSLATE_NOOP("FRelation","Children"),QString::number(this->children.size()));

    if (this->begin.isValid())
    {
        if (!shortString.isEmpty())
        {
            shortString += " ";
        }
        shortString += "(" + this->begin.getDate().toDate().toString() + ")";
    }

    return shortString;
}

void FRelation::print() const
{
    RLogger::indent();
    RLogger::info("%s: \"%s\"\n",FRelation::ElementDesc::id.toUtf8().constData(),this->id.toString(QUuid::WithoutBraces).toUtf8().constData());
    RLogger::info("%s:\n",FRelation::ElementDesc::partners.toUtf8().constData());
    RLogger::indent();
    foreach (const QUuid &partnerId, this->partners)
    {
        RLogger::info("ID: \"%s\"\n",partnerId.toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    RLogger::unindent(false);
    RLogger::info("%s:\n",FRelation::ElementDesc::children.toUtf8().constData());
    RLogger::indent();
    foreach (const QUuid &childId, this->children)
    {
        RLogger::info("ID: \"%s\"\n",childId.toString(QUuid::WithoutBraces).toUtf8().constData());
    }
    RLogger::unindent(false);
    RLogger::info("%s:\n",FRelation::ElementDesc::begin.toUtf8().constData());
    this->begin.print();
    RLogger::info("%s:\n",FRelation::ElementDesc::end.toUtf8().constData());
    this->end.print();
    RLogger::unindent(false);
}

void FRelation::readXmlElement(QXmlStreamReader &stream, const QString &tag)
{
    if (!stream.isStartElement() || stream.qualifiedName().toString() != tag)
    {
        stream.raiseError(QObject::tr("Missing element") + " <" + tag + ">.");
    }
    if (!stream.attributes().hasAttribute(FRelation::ElementName::id))
    {
        stream.raiseError(QObject::tr("Missing attribute") + " \'" + FRelation::ElementName::id + "\' in element <" + tag + ">.");
    }
    else
    {
        this->setId(QUuid(stream.attributes().value(FRelation::ElementName::id).toString()));
    }
    while(stream.readNextStartElement())
    {
        if (stream.qualifiedName().toString() == FRelation::ElementName::partners)
        {
            while(stream.readNextStartElement())
            {
                if (stream.qualifiedName().toString() == FRelation::ElementName::partnerId)
                {
                    this->partners.append(QUuid(stream.readElementText()));
                }
                else
                {
                    stream.raiseError(QObject::tr("Unexpected element") + " <" + stream.qualifiedName().toString() + ">.");
                }
            }
        }
        else if (stream.qualifiedName().toString() == FRelation::ElementName::children)
        {
            while(stream.readNextStartElement())
            {
                if (stream.qualifiedName().toString() == FRelation::ElementName::childId)
                {
                    this->children.append(QUuid(stream.readElementText()));
                }
                else
                {
                    stream.raiseError(QObject::tr("Unexpected element") + " <" + stream.qualifiedName().toString() + ">.");
                }
            }
        }
        else if (stream.qualifiedName().toString() == FRelation::ElementName::begin)
        {
            this->begin.readXmlElement(stream,FRelation::ElementName::begin);
        }
        else if (stream.qualifiedName().toString() == FRelation::ElementName::end)
        {
            this->end.readXmlElement(stream,FRelation::ElementName::end);
        }
        else
        {
            stream.raiseError(QObject::tr("Unexpected element") + " <" + stream.qualifiedName().toString() + ">.");
        }
    }
}

void FRelation::writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const
{
    stream.writeStartElement(tag);
    stream.writeAttribute(FRelation::ElementName::id,this->getId().toString(QUuid::WithoutBraces));
    stream.writeStartElement(FRelation::ElementName::partners);
    foreach (const QUuid &partnerId, this->partners)
    {
        stream.writeTextElement(FRelation::ElementName::partnerId,partnerId.toString(QUuid::WithoutBraces));
    }
    stream.writeEndElement();
    stream.writeStartElement(FRelation::ElementName::children);
    foreach (const QUuid &childId, this->children)
    {
        stream.writeTextElement(FRelation::ElementName::childId,childId.toString(QUuid::WithoutBraces));
    }
    stream.writeEndElement();
    this->begin.writeXmlElement(stream,FRelation::ElementName::begin);
    this->end.writeXmlElement(stream,FRelation::ElementName::end);
    stream.writeEndElement();
}

FRelation FRelation::fromJson(const QJsonObject &json)
{
    FRelation relation;

    if (const QJsonValue &v = json[FRelation::ElementName::id]; v.isString())
    {
        relation.id = QUuid(v.toString());
    }
    if (const QJsonValue &v = json[FRelation::ElementName::partners]; v.isArray())
    {
        const QJsonArray &jsonArray = v.toArray();

        relation.partners.reserve(jsonArray.size());
        for (const QJsonValue &jsonValue : jsonArray)
        {
            if (jsonValue.isString())
            {
                relation.partners.append(QUuid(jsonValue.toString()));
            }
        }
    }
    if (const QJsonValue &v = json[FRelation::ElementName::children]; v.isArray())
    {
        const QJsonArray &jsonArray = v.toArray();

        relation.children.reserve(jsonArray.size());
        for (const QJsonValue &jsonValue : jsonArray)
        {
            if (jsonValue.isString())
            {
                relation.children.append(QUuid(jsonValue.toString()));
            }
        }
    }
    if (const QJsonValue &v = json[FRelation::ElementName::begin]; v.isObject())
    {
        relation.begin = FEvent::fromJson(v.toObject());
    }
    if (const QJsonValue &v = json[FRelation::ElementName::end]; v.isObject())
    {
        relation.end = FEvent::fromJson(v.toObject());
    }

    return relation;
}

QJsonObject FRelation::toJson() const
{
    QJsonObject json;

    json[FRelation::ElementName::id] = this->id.toString(QUuid::WithoutBraces);
    QJsonArray partnersArray;
    for (const QUuid &partnerId : this->partners)
    {
        partnersArray.append(partnerId.toString(QUuid::WithoutBraces));
    }
    if (!partnersArray.empty()) json[FRelation::ElementName::partners] = partnersArray;
    QJsonArray childrenArray;
    for (const QUuid &partnerId : this->children)
    {
        childrenArray.append(partnerId.toString(QUuid::WithoutBraces));
    }
    if (!childrenArray.empty()) json[FRelation::ElementName::children] = childrenArray;
    if (!this->begin.isEmpty()) json[FRelation::ElementName::begin] = this->begin.toJson();
    if (!this->end.isEmpty()) json[FRelation::ElementName::end] = this->end.toJson();

    return json;
}

int FRelation::compare(const FRelation &a, const FRelation &b, const QString &sortPath)
{
    QStringList sortPathList = sortPath.split(QLatin1Char('/'),Qt::SkipEmptyParts);
    if (sortPathList.size() > 0)
    {
        if (sortPathList.at(0) == FRelation::ElementName::partners)
        {
            QList aIds = a.partners;
            QList bIds = b.partners;
            std::sort(aIds.begin(),aIds.end());
            std::sort(bIds.begin(),bIds.end());
            if (aIds < bIds)
            {
                return -1;
            }
            else if (aIds > bIds)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else if (sortPathList.at(0) == FRelation::ElementName::children)
        {
            QList aIds = a.children;
            QList bIds = b.children;
            std::sort(aIds.begin(),aIds.end());
            std::sort(bIds.begin(),bIds.end());
            if (aIds < bIds)
            {
                return -1;
            }
            else if (aIds > bIds)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else if (sortPathList.at(0) == FRelation::ElementName::begin)
        {
            sortPathList.pop_front();
            return FEvent::compare(a.begin,b.begin,sortPathList.join(QLatin1Char('/')));
        }
        else if (sortPathList.at(0) == FRelation::ElementName::end)
        {
            sortPathList.pop_front();
            return FEvent::compare(a.end,b.end,sortPathList.join(QLatin1Char('/')));
        }
    }
    if (a.id < b.id)
    {
        return -1;
    }
    else if (a.id > b.id)
    {
        return 1;
    }
    return 0;
}

bool FRelation::compareListContents(const QList<QUuid> &a, const QList<QUuid> &b)
{
    QList<QUuid> aSorted(a);
    QList<QUuid> bSorted(b);

    std::sort(aSorted.begin(),aSorted.end());
    std::sort(bSorted.begin(),bSorted.end());

    return (aSorted == bSorted);
}

