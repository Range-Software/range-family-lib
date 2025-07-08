#include <rbl_logger.h>

#include "rfl_name.h"

const QString FName::ElementName::firstName("first-name");
const QString FName::ElementName::middleName("middle-name");
const QString FName::ElementName::lastName("last-name");
const QString FName::ElementName::maidenName("maiden-name");

const QString FName::ElementDesc::firstName(QT_TRANSLATE_NOOP("FName","First name"));
const QString FName::ElementDesc::middleName(QT_TRANSLATE_NOOP("FName","Middle name"));
const QString FName::ElementDesc::lastName(QT_TRANSLATE_NOOP("FName","Last name"));
const QString FName::ElementDesc::maidenName(QT_TRANSLATE_NOOP("FName","Maiden name"));

void FName::_init(const FName *pName)
{
    if (pName)
    {
        this->firstName = pName->firstName;
        this->middleName = pName->middleName;
        this->lastName = pName->lastName;
        this->maidenName = pName->maidenName;
    }
}

FName::FName()
{
    this->_init();
}

FName::FName(const QString &firstName, const QString &middleName, const QString &lastName, const QString &maidenName)
    : firstName(firstName)
    , middleName(middleName)
    , lastName(lastName)
    , maidenName(maidenName)
{
    this->_init();
}

FName::FName(const FName &name)
{
    this->_init(&name);
}

FName::~FName()
{

}

FName &FName::operator =(const FName &name)
{
    this->_init(&name);
    return (*this);
}

bool FName::operator ==(const FName &name) const
{
    return (this->firstName == name.firstName &&
            this->middleName == name.middleName &&
            this->lastName == name.lastName &&
            this->maidenName == name.maidenName);
}

void FName::update(const FName &name)
{
    if (!name.firstName.isNull())
    {
        this->firstName = name.firstName;
    }
    if (!name.middleName.isNull())
    {
        this->middleName = name.middleName;
    }
    if (!name.lastName.isNull())
    {
        this->lastName = name.lastName;
    }
    if (!name.maidenName.isNull())
    {
        this->maidenName = name.maidenName;
    }
}

bool FName::isEmpty() const
{
    return this->firstName.isEmpty() && this->middleName.isEmpty() && this->lastName.isEmpty() && this->maidenName.isEmpty();
}

bool FName::isValid() const
{
    return true;
}

void FName::validate() const
{
}

const QString &FName::getFirstName() const
{
    return this->firstName;
}

void FName::setFirstName(const QString &firstName)
{
    this->firstName = firstName;
}

const QString &FName::getMiddleName() const
{
    return this->middleName;
}

void FName::setMiddleName(const QString &middleName)
{
    this->middleName = middleName;
}

const QString &FName::getLastName() const
{
    return this->lastName;
}

void FName::setLastName(const QString &lastName)
{
    this->lastName = lastName;
}

const QString &FName::getMaidenName() const
{
    return this->maidenName;
}

void FName::setMaidenName(const QString &maidenName)
{
    this->maidenName = maidenName;
}

void FName::merge(const FName &name)
{
    if (this->firstName.isEmpty())
    {
        this->firstName = name.getFirstName();
    }
    if (this->middleName.isEmpty())
    {
        this->middleName = name.getMiddleName();
    }
    if (this->lastName.isEmpty())
    {
        this->lastName = name.getLastName();
    }
    if (this->maidenName.isEmpty())
    {
        this->maidenName = name.getMaidenName();
    }
}

QString FName::toString() const
{
    QString fullName;

    if (!this->firstName.isEmpty())
    {
        fullName = this->firstName;
    }
    if (!this->middleName.isEmpty())
    {
        if (!fullName.isEmpty())
        {
            fullName += " ";
        }
        fullName += this->middleName;
    }
    if (!this->lastName.isEmpty())
    {
        if (!fullName.isEmpty())
        {
            fullName += " ";
        }
        fullName += this->lastName;
    }
    if (!this->maidenName.isEmpty())
    {
        if (!fullName.isEmpty())
        {
            fullName += " ";
        }
        fullName += "(" + this->maidenName + ")";
    }

    return fullName;
}

void FName::print() const
{
    RLogger::indent();
    RLogger::info("%s: \"%s\"\n",FName::ElementDesc::firstName.toUtf8().constData(),this->firstName.toUtf8().constData());
    RLogger::info("%s: \"%s\"\n",FName::ElementDesc::middleName.toUtf8().constData(),this->middleName.toUtf8().constData());
    RLogger::info("%s: \"%s\"\n",FName::ElementDesc::lastName.toUtf8().constData(),this->lastName.toUtf8().constData());
    RLogger::info("%s: \"%s\"\n",FName::ElementDesc::maidenName.toUtf8().constData(),this->maidenName.toUtf8().constData());
    RLogger::unindent(false);
}

void FName::readXmlElement(QXmlStreamReader &stream, const QString &tag)
{
    if (!stream.isStartElement() || stream.qualifiedName().toString() != tag)
    {
        stream.raiseError(QObject::tr("Missing element") + " <" + tag + ">.");
    }
    while(stream.readNextStartElement())
    {
        if (stream.qualifiedName().toString() == FName::ElementName::firstName)
        {
            this->firstName = stream.readElementText();
        }
        else if (stream.qualifiedName().toString() == FName::ElementName::middleName)
        {
            this->middleName = stream.readElementText();
        }
        else if (stream.qualifiedName().toString() == FName::ElementName::lastName)
        {
            this->lastName = stream.readElementText();
        }
        else if (stream.qualifiedName().toString() == FName::ElementName::maidenName)
        {
            this->maidenName = stream.readElementText();
        }
        else
        {
            stream.raiseError(QObject::tr("Unexpected element") + " <" + stream.qualifiedName().toString() + ">.");
        }
    }
}

void FName::writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const
{
    stream.writeStartElement(tag);
    stream.writeTextElement(FName::ElementName::firstName,this->firstName);
    stream.writeTextElement(FName::ElementName::middleName,this->middleName);
    stream.writeTextElement(FName::ElementName::lastName,this->lastName);
    stream.writeTextElement(FName::ElementName::maidenName,this->maidenName);
    stream.writeEndElement();
}

FName FName::fromJson(const QJsonObject &json)
{
    FName name;

    if (const QJsonValue &v = json[FName::ElementName::firstName]; v.isString())
    {
        name.firstName = v.toString();
    }
    if (const QJsonValue &v = json[FName::ElementName::middleName]; v.isString())
    {
        name.middleName = v.toString();
    }
    if (const QJsonValue &v = json[FName::ElementName::lastName]; v.isString())
    {
        name.lastName = v.toString();
    }
    if (const QJsonValue &v = json[FName::ElementName::maidenName]; v.isString())
    {
        name.maidenName = v.toString();
    }

    return name;
}

QJsonObject FName::toJson() const
{
    QJsonObject json;

    if (!this->firstName.isEmpty()) json[FName::ElementName::firstName] = this->firstName;
    if (!this->middleName.isEmpty()) json[FName::ElementName::middleName] = this->middleName;
    if (!this->lastName.isEmpty()) json[FName::ElementName::lastName] = this->lastName;
    if (!this->maidenName.isEmpty()) json[FName::ElementName::maidenName] = this->maidenName;

    return json;
}

int FName::compare(const FName &a, const FName &b, const QString &sortPath)
{
    QStringList sortPathList = sortPath.split(QLatin1Char('/'),Qt::SkipEmptyParts);
    if (sortPathList.size() > 0)
    {
        if (sortPathList.at(0) == FName::ElementName::firstName)
        {
            return QString::compare(a.firstName,b.firstName);
        }
        else if (sortPathList.at(0) == FName::ElementName::middleName)
        {
            return QString::compare(a.middleName,b.middleName);
        }
        else if (sortPathList.at(0) == FName::ElementName::lastName)
        {
            return QString::compare(a.lastName,b.lastName);
        }
        else if (sortPathList.at(0) == FName::ElementName::maidenName)
        {
            return QString::compare(a.maidenName,b.maidenName);
        }
    }
    return QString::compare(a.toString(),b.toString());
}
