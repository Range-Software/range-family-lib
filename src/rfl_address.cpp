#include <rbl_logger.h>

#include "rfl_address.h"

const QString FAddress::ElementName::street("street");
const QString FAddress::ElementName::streetNumber("street-number");
const QString FAddress::ElementName::city("city");
const QString FAddress::ElementName::postalCode("postal-code");
const QString FAddress::ElementName::country("country");

const QString FAddress::ElementDesc::street(QT_TRANSLATE_NOOP("FAddress","Street"));
const QString FAddress::ElementDesc::streetNumber(QT_TRANSLATE_NOOP("FAddress","Street number"));
const QString FAddress::ElementDesc::city(QT_TRANSLATE_NOOP("FAddress","City"));
const QString FAddress::ElementDesc::postalCode(QT_TRANSLATE_NOOP("FAddress","Postal code"));
const QString FAddress::ElementDesc::country(QT_TRANSLATE_NOOP("FAddress","Country"));

void FAddress::_init(const FAddress *pAddress)
{
    if (pAddress)
    {
        this->street = pAddress->street;
        this->streetNumber = pAddress->streetNumber;
        this->city = pAddress->city;
        this->postalCode = pAddress->postalCode;
        this->country = pAddress->country;
    }
}

FAddress::FAddress()
{
    this->_init();
}

FAddress::FAddress(const QString &street, const QString &streetNumber, const QString &city, const QString &postalCode, const QString &country)
    : street(street)
    , streetNumber(streetNumber)
    , city(city)
    , postalCode(postalCode)
    , country(country)
{
    this->_init();
}

FAddress::FAddress(const FAddress &address)
{
    this->_init(&address);
}

FAddress::~FAddress()
{

}

FAddress &FAddress::operator =(const FAddress &address)
{
    this->_init(&address);
    return (*this);
}

bool FAddress::operator ==(const FAddress &address) const
{
    return (this->street == address.street &&
            this->streetNumber == address.streetNumber &&
            this->city == address.city &&
            this->postalCode == address.postalCode &&
            this->country == address.country);
}

void FAddress::update(const FAddress &address)
{
    if (!address.street.isNull())
    {
        this->street = address.street;
    }
    if (!address.streetNumber.isNull())
    {
        this->streetNumber = address.streetNumber;
    }
    if (!address.city.isNull())
    {
        this->city = address.city;
    }
    if (!address.postalCode.isNull())
    {
        this->postalCode = address.postalCode;
    }
    if (!address.country.isNull())
    {
        this->country = address.country;
    }
}

bool FAddress::isEmpty() const
{
    return this->street.isEmpty() && \
           this->streetNumber.isEmpty() && \
           this->city.isEmpty() && \
           this->postalCode.isEmpty() && \
           this->country.isEmpty();
}

bool FAddress::isValid() const
{
    return true;
}

void FAddress::validate() const
{
}

const QString &FAddress::getStreet() const
{
    return this->street;
}

void FAddress::setStreet(const QString &street)
{
    this->street = street;
}

const QString &FAddress::getStreetNumber() const
{
    return this->streetNumber;
}

void FAddress::setStreetNumber(const QString &streetNumber)
{
    this->streetNumber = streetNumber;
}

const QString &FAddress::getCity() const
{
    return this->city;
}

void FAddress::setCity(const QString &city)
{
    this->city = city;
}

const QString &FAddress::getPostalCode() const
{
    return this->postalCode;
}

void FAddress::setPostalCode(const QString &postalCode)
{
    this->postalCode = postalCode;
}

const QString &FAddress::getCountry() const
{
    return this->country;
}

void FAddress::setCountry(const QString &country)
{
    this->country = country;
}

void FAddress::merge(const FAddress &address)
{
    if (this->street.isEmpty())
    {
        this->street = address.getStreet();
    }
    if (this->streetNumber.isEmpty())
    {
        this->streetNumber = address.getStreetNumber();
    }
    if (this->city.isEmpty())
    {
        this->city = address.getCity();
    }
    if (this->postalCode.isEmpty())
    {
        this->postalCode = address.getPostalCode();
    }
    if (this->country.isEmpty())
    {
        this->country = address.getCountry();
    }
}

QString FAddress::toString() const
{
    QString fullAddress;

    if (!this->street.isEmpty())
    {
        fullAddress = this->street;
    }

    if (!this->streetNumber.isEmpty())
    {
        if (!fullAddress.isEmpty())
        {
            fullAddress += " ";
        }
        fullAddress += this->streetNumber;
    }

    if (!this->city.isEmpty())
    {
        if (!fullAddress.isEmpty())
        {
            fullAddress += ", ";
        }
        fullAddress += this->city;
    }

    if (!this->postalCode.isEmpty())
    {
        if (!fullAddress.isEmpty())
        {
            fullAddress += ", ";
        }
        fullAddress += this->postalCode;
    }

    if (!this->country.isEmpty())
    {
        if (!fullAddress.isEmpty())
        {
            fullAddress += ", ";
        }
        fullAddress += this->country;
    }

    return fullAddress;
}

void FAddress::print() const
{
    RLogger::indent();
    RLogger::info("%s: \"%s\"\n",FAddress::ElementDesc::street.toUtf8().constData(),this->street.toUtf8().constData());
    RLogger::info("%s: \"%s\"\n",FAddress::ElementDesc::streetNumber.toUtf8().constData(),this->streetNumber.toUtf8().constData());
    RLogger::info("%s: \"%s\"\n",FAddress::ElementDesc::city.toUtf8().constData(),this->city.toUtf8().constData());
    RLogger::info("%s: \"%s\"\n",FAddress::ElementDesc::postalCode.toUtf8().constData(),this->postalCode.toUtf8().constData());
    RLogger::info("%s: \"%s\"\n",FAddress::ElementDesc::country.toUtf8().constData(),this->country.toUtf8().constData());
    RLogger::unindent(false);
}

void FAddress::readXmlElement(QXmlStreamReader &stream, const QString &tag)
{
    if (!stream.isStartElement() || stream.qualifiedName().toString() != tag)
    {
        stream.raiseError(QObject::tr("Missing element") + " <" + tag + ">.");
    }
    while(stream.readNextStartElement())
    {
        if (stream.qualifiedName().toString() == FAddress::ElementName::street)
        {
            this->street = stream.readElementText();
        }
        else if (stream.qualifiedName().toString() == FAddress::ElementName::streetNumber)
        {
            this->streetNumber = stream.readElementText();
        }
        else if (stream.qualifiedName().toString() == FAddress::ElementName::city)
        {
            this->city = stream.readElementText();
        }
        else if (stream.qualifiedName().toString() == FAddress::ElementName::postalCode)
        {
            this->postalCode = stream.readElementText();
        }
        else if (stream.qualifiedName().toString() == FAddress::ElementName::country)
        {
            this->country = stream.readElementText();
        }
        else
        {
            stream.raiseError(QObject::tr("Unexpected element") + " <" + stream.qualifiedName().toString() + ">.");
        }
    }
}

void FAddress::writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const
{
    stream.writeStartElement(tag);
    stream.writeTextElement(FAddress::ElementName::street,this->street);
    stream.writeTextElement(FAddress::ElementName::streetNumber,this->streetNumber);
    stream.writeTextElement(FAddress::ElementName::city,this->city);
    stream.writeTextElement(FAddress::ElementName::postalCode,this->postalCode);
    stream.writeTextElement(FAddress::ElementName::country,this->country);
    stream.writeEndElement();
}

FAddress FAddress::fromJson(const QJsonObject &json)
{
    FAddress address;

    if (const QJsonValue &v = json[FAddress::ElementName::street]; v.isString())
    {
        address.street = v.toString();
    }
    if (const QJsonValue &v = json[FAddress::ElementName::streetNumber]; v.isString())
    {
        address.streetNumber = v.toString();
    }
    if (const QJsonValue &v = json[FAddress::ElementName::city]; v.isString())
    {
        address.city = v.toString();
    }
    if (const QJsonValue &v = json[FAddress::ElementName::postalCode]; v.isString())
    {
        address.postalCode = v.toString();
    }
    if (const QJsonValue &v = json[FAddress::ElementName::country]; v.isString())
    {
        address.country = v.toString();
    }

    return address;
}

QJsonObject FAddress::toJson() const
{
    QJsonObject json;

    if (!this->street.isEmpty()) json[FAddress::ElementName::street] = this->street;
    if (!this->streetNumber.isEmpty()) json[FAddress::ElementName::streetNumber] = this->streetNumber;
    if (!this->city.isEmpty()) json[FAddress::ElementName::city] = this->city;
    if (!this->postalCode.isEmpty()) json[FAddress::ElementName::postalCode] = this->postalCode;
    if (!this->country.isEmpty()) json[FAddress::ElementName::country] = this->country;

    return json;
}

int FAddress::compare(const FAddress &a, const FAddress &b, const QString &sortPath)
{
    QStringList sortPathList = sortPath.split(QLatin1Char('/'),Qt::SkipEmptyParts);
    if (sortPathList.size() > 0)
    {
        if (sortPathList.at(0) == FAddress::ElementName::street)
        {
            return QString::compare(a.street,b.street);
        }
        else if (sortPathList.at(0) == FAddress::ElementName::streetNumber)
        {
            return QString::compare(a.streetNumber,b.streetNumber);
        }
        else if (sortPathList.at(0) == FAddress::ElementName::city)
        {
            return QString::compare(a.city,b.city);
        }
        else if (sortPathList.at(0) == FAddress::ElementName::postalCode)
        {
            return QString::compare(a.postalCode,b.postalCode);
        }
        else if (sortPathList.at(0) == FAddress::ElementName::country)
        {
            return QString::compare(a.country,b.country);
        }
    }
    return QString::compare(a.toString(),b.toString());
}
