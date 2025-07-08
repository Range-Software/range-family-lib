#include <rbl_logger.h>

#include "rfl_place.h"

const QString FPlace::ElementName::address("address");
const QString FPlace::ElementName::geoCoordinate("geo-coordinate");

const QString FPlace::ElementDesc::address(QT_TRANSLATE_NOOP("FPlace","Address"));
const QString FPlace::ElementDesc::geoCoordinate(QT_TRANSLATE_NOOP("FPlace","Coordinate"));

void FPlace::_init(const FPlace *pPlace)
{
    if (pPlace)
    {
        this->address = pPlace->address;
        this->geoCoordinate = pPlace->geoCoordinate;
    }
}

FPlace::FPlace()
{
    this->_init();
}

FPlace::FPlace(const FAddress &address, const FGeoCoordinate &geoCoordinates)
    : address(address)
    , geoCoordinate(geoCoordinates)
{
    this->_init();
}

FPlace::FPlace(const FPlace &place)
{
    this->_init(&place);
}

FPlace::~FPlace()
{

}

FPlace &FPlace::operator =(const FPlace &place)
{
    this->_init(&place);
    return (*this);
}

bool FPlace::operator ==(const FPlace &place) const
{
    return (this->address == place.address &&
            this->geoCoordinate == place.geoCoordinate);
}

void FPlace::update(const FPlace &place)
{
    this->address.update(place.address);
    this->geoCoordinate.update(place.geoCoordinate);
}

bool FPlace::isEmpty() const
{
    return this->address.isEmpty() && this->geoCoordinate.isEmpty();
}

bool FPlace::isValid() const
{
    return this->address.isValid() && this->geoCoordinate.isValid();
}

void FPlace::validate() const
{
    this->address.validate();
    this->geoCoordinate.validate();
}

const FAddress &FPlace::getAddress() const
{
    return this->address;
}

void FPlace::setAddress(const FAddress &address)
{
    this->address = address;
}

const FGeoCoordinate &FPlace::getGeoCoordinates() const
{
    return this->geoCoordinate;
}

void FPlace::setGeoCoordinates(const FGeoCoordinate &geoCoordinates)
{
    this->geoCoordinate = geoCoordinates;
}

void FPlace::merge(const FPlace &place)
{
    this->address.merge(place.getAddress());
    this->geoCoordinate.merge(place.getGeoCoordinates());
}

QString FPlace::toString() const
{
    return this->address.toString() + "/" + this->geoCoordinate.toString();
}

void FPlace::print() const
{
    RLogger::indent();
    RLogger::info("%s:\n",FPlace::ElementDesc::address.toUtf8().constData());
    this->address.print();
    RLogger::info("%s:\n",FPlace::ElementDesc::geoCoordinate.toUtf8().constData());
    this->geoCoordinate.print();
    RLogger::unindent(false);
}

void FPlace::readXmlElement(QXmlStreamReader &stream, const QString &tag)
{
    if (!stream.isStartElement() || stream.qualifiedName().toString() != tag)
    {
        stream.raiseError(QObject::tr("Missing element") + " <" + tag + ">.");
    }
    while(stream.readNextStartElement())
    {
        if (stream.qualifiedName().toString() == FPlace::ElementName::address)
        {
            this->address.readXmlElement(stream,FPlace::ElementName::address);
        }
        else if (stream.qualifiedName().toString() == FPlace::ElementName::geoCoordinate)
        {
            this->geoCoordinate.readXmlElement(stream,FPlace::ElementName::geoCoordinate);
        }
        else
        {
            stream.raiseError(QObject::tr("Unexpected element") + " <" + stream.qualifiedName().toString() + ">.");
        }
    }
}

void FPlace::writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const
{
    stream.writeStartElement(tag);
    this->address.writeXmlElement(stream,FPlace::ElementName::address);
    this->geoCoordinate.writeXmlElement(stream,FPlace::ElementName::geoCoordinate);
    stream.writeEndElement();
}

FPlace FPlace::fromJson(const QJsonObject &json)
{
    FPlace place;

    if (const QJsonValue &v = json[FPlace::ElementName::address]; v.isObject())
    {
        place.address = FAddress::fromJson(v.toObject());
    }
    if (const QJsonValue &v = json[FPlace::ElementName::geoCoordinate]; v.isObject())
    {
        place.geoCoordinate = FGeoCoordinate::fromJson(v.toObject());
    }

    return place;
}

QJsonObject FPlace::toJson() const
{
    QJsonObject json;

    if (!this->address.isEmpty()) json[FPlace::ElementName::address] = this->address.toJson();
    if (!this->geoCoordinate.isEmpty()) json[FPlace::ElementName::geoCoordinate] = this->geoCoordinate.toJson();

    return json;
}

int FPlace::compare(const FPlace &a, const FPlace &b, const QString &sortPath)
{
    QStringList sortPathList = sortPath.split(QLatin1Char('/'),Qt::SkipEmptyParts);
    if (sortPathList.size() > 0)
    {
        if (sortPathList.at(0) == FPlace::ElementName::address)
        {
            sortPathList.pop_front();
            return FAddress::compare(a.address,b.address,sortPathList.join(QLatin1Char('/')));
        }
        else if (sortPathList.at(0) == FPlace::ElementName::geoCoordinate)
        {
            sortPathList.pop_front();
            return FGeoCoordinate::compare(a.geoCoordinate,b.geoCoordinate,sortPathList.join(QLatin1Char('/')));
        }
    }
    return QString::compare(a.toString(),b.toString());
}
