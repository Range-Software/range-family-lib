#include <rbl_logger.h>

#include "rfl_person.h"

const QString FPerson::ElementName::id("id");
const QString FPerson::ElementName::sex("sex");
const QString FPerson::ElementName::name("name");
const QString FPerson::ElementName::birth("birth");
const QString FPerson::ElementName::death("death");
const QString FPerson::ElementName::text("text");
const QString FPerson::ElementName::picture("picture");

const QString FPerson::ElementDesc::id(QT_TRANSLATE_NOOP("FPerson","ID"));
const QString FPerson::ElementDesc::sex(QT_TRANSLATE_NOOP("FPerson","Sex"));
const QString FPerson::ElementDesc::name(QT_TRANSLATE_NOOP("FPerson","Name"));
const QString FPerson::ElementDesc::birth(QT_TRANSLATE_NOOP("FPerson","Birth"));
const QString FPerson::ElementDesc::death(QT_TRANSLATE_NOOP("FPerson","Death"));
const QString FPerson::ElementDesc::text(QT_TRANSLATE_NOOP("FPerson","Text"));
const QString FPerson::ElementDesc::picture(QT_TRANSLATE_NOOP("FPerson","Picture"));

const QString FPerson::SexDesc::male(QT_TRANSLATE_NOOP("FPerson","Male"));
const QString FPerson::SexDesc::female(QT_TRANSLATE_NOOP("FPerson","Female"));
const QString FPerson::SexDesc::other(QT_TRANSLATE_NOOP("FPerson","Other"));

void FPerson::_init(const FPerson *pPerson)
{
    if (pPerson)
    {
        this->id = pPerson->id;
        this->sex = pPerson->sex;
        this->name = pPerson->name;
        this->birth = pPerson->birth;
        this->death = pPerson->death;
        this->text = pPerson->text;
        this->picture = pPerson->picture;
    }
}

FPerson::FPerson()
{
    this->_init();
}

FPerson::FPerson(const QUuid &id, const QString &sex, const FName &name, const FEvent &birth, const FEvent &death, const QString &text, const FPicture &picture)
    : id{id}
    , sex{sex}
    , name{name}
    , birth{birth}
    , death{death}
    , text{text}
    , picture{picture}
{
    this->_init();
}

FPerson::FPerson(const FPerson &person)
{
    this->_init(&person);
}

FPerson::~FPerson()
{
}

FPerson & FPerson::operator =(const FPerson &person)
{
    this->_init(&person);
    return (*this);
}

bool FPerson::operator ==(const FPerson &person) const
{
    return (this->id == person.id &&
            this->sex == person.sex &&
            this->name == person.name &&
            this->birth == person.birth &&
            this->death == person.death &&
            this->text == person.text &&
            this->picture == person.picture);
}

bool FPerson::operator !=(const FPerson &person) const
{
    return !((*this) == person);
}

void FPerson::update(const FPerson &person)
{
    if (!person.sex.isNull())
    {
        this->sex = person.sex;
    }
    this->name.update(person.name);
    this->birth.update(person.birth);
    this->death.update(person.death);
}

bool FPerson::isEmpty() const
{
    return this->id.isNull() &&
           this->sex.isEmpty() &&
           this->name.isEmpty() &&
           this->birth.isEmpty() &&
           this->death.isEmpty() &&
           this->text.isEmpty() &&
           this->picture.isEmpty();
}

bool FPerson::isValid() const
{
    return this->name.isValid() && this->birth.isValid() && this->death.isValid();
}

void FPerson::validate() const
{
    this->name.validate();
    this->birth.validate();
    this->death.validate();
}

const QUuid &FPerson::getId() const
{
    return this->id;
}

void FPerson::setId(const QUuid &id)
{
    this->id = id;
}

const FName &FPerson::getName() const
{
    return this->name;
}

void FPerson::setSex(const QString &sex)
{
    this->sex = sex;
}

const QString &FPerson::getSex() const
{
    return this->sex;
}

void FPerson::setName(const FName &name)
{
    this->name = name;
}

const FEvent &FPerson::getBirth() const
{
    return this->birth;
}

void FPerson::setBirth(const FEvent &birth)
{
    this->birth = birth;
}

const FEvent &FPerson::getDeath() const
{
    return this->death;
}

void FPerson::setDeath(const FEvent &death)
{
    this->death = death;
}

const QString &FPerson::getText() const
{
    return this->text;
}

void FPerson::setText(const QString &text)
{
    this->text = text;
}

const FPicture &FPerson::getPicture() const
{
    return this->picture;
}

void FPerson::setPicture(const FPicture &picture)
{
    this->picture = picture;
}

void FPerson::merge(const FPerson &person)
{
    if (this->sex.isEmpty())
    {
        this->sex = person.getSex();
    }
    this->name.merge(person.getName());
    this->birth.merge(person.getBirth());
    this->death.merge(person.getDeath());
    if (!this->text.isEmpty())
    {
        this->text += "\n----\n";
    }
    this->text += person.getText();
    if (this->picture.isEmpty())
    {
        this->picture = person.getPicture();
    }
}

QString FPerson::toString() const
{
    return this->sex + "/"
         + this->name.toString() + "/"
         + this->birth.toString() + "/"
         + this->death.toString();
}

QString FPerson::toShortString() const
{
    QString shortString;

    if (!this->name.getFirstName().isEmpty())
    {
        if (!shortString.isEmpty())
        {
            shortString += " ";
        }
        shortString += this->name.getFirstName();
    }

    if (!this->name.getMiddleName().isEmpty())
    {
        if (!shortString.isEmpty())
        {
            shortString += " ";
        }
        shortString += this->name.getMiddleName();
    }

    if (!this->name.getLastName().isEmpty())
    {
        if (!shortString.isEmpty())
        {
            shortString += " ";
        }
        shortString += this->name.getLastName();
    }

    if (this->birth.isValid())
    {
        if (!shortString.isEmpty())
        {
            shortString += " ";
        }
        shortString += "(" + this->birth.getDate().toDate().toString() + ")";
    }

    return shortString;
}

void FPerson::print() const
{
    RLogger::indent();
    RLogger::info("%s: \"%s\"\n",FPerson::ElementDesc::id.toUtf8().constData(),this->id.toString(QUuid::WithoutBraces).toUtf8().constData());
    RLogger::info("%s: \"%s\"\n",FPerson::ElementDesc::sex.toUtf8().constData(),this->sex.toUtf8().constData());
    RLogger::info("%s:\n",FPerson::ElementDesc::name.toUtf8().constData(),this->sex.toUtf8().constData());
    this->name.print();
    RLogger::info("%s:\n",FPerson::ElementDesc::birth.toUtf8().constData());
    this->birth.print();
    RLogger::info("%s:\n",FPerson::ElementDesc::death.toUtf8().constData());
    this->death.print();
    RLogger::info("%s: \"%s\"\n",FPerson::ElementDesc::text.toUtf8().constData(),this->text.toUtf8().constData());
    RLogger::unindent(false);
}

void FPerson::readXmlElement(QXmlStreamReader &stream, const QString &tag)
{
    if (!stream.isStartElement() || stream.qualifiedName().toString() != tag)
    {
        stream.raiseError(QObject::tr("Missing element") + " <" + tag + ">.");
    }
    if (!stream.attributes().hasAttribute(FPerson::ElementName::id))
    {
        stream.raiseError(QObject::tr("Missing attribute") + " \'" + FPerson::ElementName::id + "\' in element <" + tag + ">.");
    }
    else
    {
        this->setId(QUuid(stream.attributes().value(FPerson::ElementName::id).toString()));
    }
    while(stream.readNextStartElement())
    {
        if (stream.qualifiedName().toString() == FPerson::ElementName::sex)
        {
            this->sex = stream.readElementText();
        }
        else if (stream.qualifiedName().toString() == FPerson::ElementName::name)
        {
            this->name.readXmlElement(stream,FPerson::ElementName::name);
        }
        else if (stream.qualifiedName().toString() == FPerson::ElementName::birth)
        {
            this->birth.readXmlElement(stream,FPerson::ElementName::birth);
        }
        else if (stream.qualifiedName().toString() == FPerson::ElementName::death)
        {
            this->death.readXmlElement(stream,FPerson::ElementName::death);
        }
        else if (stream.qualifiedName().toString() == FPerson::ElementName::text)
        {
            this->text = stream.readElementText();
        }
        else if (stream.qualifiedName().toString() == FPerson::ElementName::picture)
        {
            this->picture.readXmlElement(stream,FPerson::ElementName::picture);
        }
        else
        {
            stream.raiseError(QObject::tr("Unexpected element") + " <" + stream.qualifiedName().toString() + ">.");
        }
    }
}

void FPerson::writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const
{
    stream.writeStartElement(tag);
    stream.writeAttribute(FPerson::ElementName::id,this->getId().toString(QUuid::WithoutBraces));
    stream.writeTextElement(FPerson::ElementName::sex,this->sex);
    this->name.writeXmlElement(stream,FPerson::ElementName::name);
    this->birth.writeXmlElement(stream,FPerson::ElementName::birth);
    this->death.writeXmlElement(stream,FPerson::ElementName::death);
    stream.writeTextElement(FPerson::ElementName::text,this->text);
    this->picture.writeXmlElement(stream,FPerson::ElementName::picture);
    stream.writeEndElement(); // person
}

FPerson FPerson::fromJson(const QJsonObject &json)
{
    FPerson person;

    if (const QJsonValue &v = json[FPerson::ElementName::id]; v.isString())
    {
        person.id = QUuid(v.toString());
    }
    if (const QJsonValue &v = json[FPerson::ElementName::sex]; v.isString())
    {
        person.sex = v.toString();
    }
    if (const QJsonValue &v = json[FPerson::ElementName::name]; v.isObject())
    {
        person.name = FName::fromJson(v.toObject());
    }
    if (const QJsonValue &v = json[FPerson::ElementName::birth]; v.isObject())
    {
        person.birth = FEvent::fromJson(v.toObject());
    }
    if (const QJsonValue &v = json[FPerson::ElementName::death]; v.isObject())
    {
        person.death = FEvent::fromJson(v.toObject());
    }
    if (const QJsonValue &v = json[FPerson::ElementName::text]; v.isString())
    {
        person.text = QByteArray::fromBase64(v.toString().toUtf8());
    }
    if (const QJsonValue &v = json[FPerson::ElementName::picture]; v.isObject())
    {
        person.picture = FPicture::fromJson(v.toObject());
    }

    return person;
}

QJsonObject FPerson::toJson() const
{
    QJsonObject json;

    json[FPerson::ElementName::id] = this->id.toString(QUuid::WithoutBraces);
    if (!this->sex.isEmpty()) json[FPerson::ElementName::sex] = this->sex;
    if (!this->name.isEmpty()) json[FPerson::ElementName::name] = this->name.toJson();
    if (!this->birth.isEmpty()) json[FPerson::ElementName::birth] = this->birth.toJson();
    if (!this->death.isEmpty()) json[FPerson::ElementName::death] = this->death.toJson();
    if (!this->text.isEmpty()) json[FPerson::ElementName::text] = this->text.toUtf8().toBase64().constData();
    if (!this->picture.isEmpty()) json[FPerson::ElementName::picture] = this->picture.toJson();

    return json;
}

int FPerson::compare(const FPerson &a, const FPerson &b, const QString &sortPath)
{
    QStringList sortPathList = sortPath.split(QLatin1Char('/'),Qt::SkipEmptyParts);
    if (sortPathList.size() > 0)
    {
        if (sortPathList.at(0) == FPerson::ElementName::sex)
        {
            return QString::compare(a.sex,b.sex);
        }
        else if (sortPathList.at(0) == FPerson::ElementName::name)
        {
            sortPathList.pop_front();
            return FName::compare(a.name,b.name,sortPathList.join(QLatin1Char('/')));
        }
        else if (sortPathList.at(0) == FPerson::ElementName::birth)
        {
            sortPathList.pop_front();
            return FEvent::compare(a.birth,b.birth,sortPathList.join(QLatin1Char('/')));
        }
        else if (sortPathList.at(0) == FPerson::ElementName::death)
        {
            sortPathList.pop_front();
            return FEvent::compare(a.death,b.death,sortPathList.join(QLatin1Char('/')));
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

int FPerson::sexDescToIndex(const QString &sexDesc)
{
    QString lSexDesc(sexDesc.toLower());
    if (lSexDesc == FPerson::SexDesc::male.toLower())
    {
        return FPerson::SexIndex::male;
    }
    else if (lSexDesc == FPerson::SexDesc::female.toLower())
    {
        return FPerson::SexIndex::female;
    }
    else if (lSexDesc == FPerson::SexDesc::other.toLower())
    {
        return FPerson::SexIndex::other;
    }
    return 0;
}

QString FPerson::sexIndexToDesc(int sexIndex)
{
    if (sexIndex == FPerson::SexIndex::male)
    {
        return FPerson::SexDesc::male;
    }
    else if (sexIndex == FPerson::SexIndex::female)
    {
        return FPerson::SexDesc::female;
    }
    else if (sexIndex == FPerson::SexIndex::other)
    {
        return FPerson::SexDesc::other;
    }
    return QString();
}

QString FPerson::sexIndexToKey(int sexIndex)
{
    return FPerson::sexIndexToDesc(sexIndex).toLower();
}

QString FPerson::sexDescToKey(const QString &sexDesc)
{
    QString lSexDesc(sexDesc.toLower());
    if (lSexDesc == FPerson::SexDesc::male.toLower() ||
        lSexDesc == FPerson::SexDesc::female.toLower() ||
        lSexDesc == FPerson::SexDesc::other.toLower())
    {
        return lSexDesc;
    }
    return QString();
}

QString FPerson::sexKeyToDesc(const QString &sexKey)
{
    QString lSexKey(sexKey.toLower());
    if (lSexKey == FPerson::SexDesc::male.toLower())
    {
        return FPerson::SexDesc::male;
    }
    else if (lSexKey == FPerson::SexDesc::female.toLower())
    {
        return FPerson::SexDesc::female;
    }
    else if (lSexKey == FPerson::SexDesc::other.toLower())
    {
        return FPerson::SexDesc::other;
    }
    return QString();
}
