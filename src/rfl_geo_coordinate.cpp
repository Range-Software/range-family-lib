#include <rbl_error.h>
#include <rbl_logger.h>

#include "rfl_geo_coordinate.h"
#include "rfl_utils.h"

const QString FGeoCoordinate::ElementName::latitude("latitude");
const QString FGeoCoordinate::ElementName::longitude("longitude");
const QString FGeoCoordinate::ElementName::altitude("altitude");

const QString FGeoCoordinate::ElementDesc::latitude(QT_TRANSLATE_NOOP("FGeoCoordinate","Latitude"));
const QString FGeoCoordinate::ElementDesc::longitude(QT_TRANSLATE_NOOP("FGeoCoordinate","Longitude"));
const QString FGeoCoordinate::ElementDesc::altitude(QT_TRANSLATE_NOOP("FGeoCoordinate","Altitude"));

const double FGeoCoordinate::minLatitude(-90.0);
const double FGeoCoordinate::maxLatitude(90.0);
const double FGeoCoordinate::minLongitude(-180.0);
const double FGeoCoordinate::maxLongitude(180.0);
const double FGeoCoordinate::minAltitude(-6371007.2);

void FGeoCoordinate::_init(const FGeoCoordinate *pGeoCoordinates)
{
    if (pGeoCoordinates)
    {
        this->latitude = pGeoCoordinates->latitude;
        this->longitude = pGeoCoordinates->longitude;
        this->altitude = pGeoCoordinates->altitude;
    }
}

FGeoCoordinate::FGeoCoordinate()
    : latitude(0.0)
    , longitude(0.0)
    , altitude(0.0)
{
    this->_init();
}

FGeoCoordinate::FGeoCoordinate(double latitude, double longitude, double altitude)
    : latitude(latitude)
    , longitude(longitude)
    , altitude(altitude)
{
    this->_init();
}

FGeoCoordinate::FGeoCoordinate(const FGeoCoordinate &geoCoordinates)
{
    this->_init(&geoCoordinates);
}

FGeoCoordinate::~FGeoCoordinate()
{

}

FGeoCoordinate &FGeoCoordinate::operator =(const FGeoCoordinate &geoCoordinates)
{
    this->_init(&geoCoordinates);
    return (*this);
}

bool FGeoCoordinate::operator ==(const FGeoCoordinate &geoCoordinates) const
{
    return (this->latitude == geoCoordinates.latitude &&
            this->longitude == geoCoordinates.longitude &&
            this->altitude == geoCoordinates.altitude);
}

void FGeoCoordinate::update(const FGeoCoordinate &geoCoordinates)
{
    if (geoCoordinates.latitude != 0.0)
    {
        this->latitude = geoCoordinates.latitude;
    }
    if (geoCoordinates.longitude != 0.0)
    {
        this->longitude = geoCoordinates.longitude;
    }
    if (geoCoordinates.altitude != 0.0)
    {
        this->altitude = geoCoordinates.altitude;
    }
}

bool FGeoCoordinate::isEmpty() const
{
    return (this->latitude == 0.0 && this->longitude == 0.0 && this->altitude == 0.0);
}

bool FGeoCoordinate::isValid() const
{
    return this->isEmpty() || \
           (R_IS_IN_CLOSED_INTERVAL(FGeoCoordinate::minLatitude,FGeoCoordinate::maxLatitude,this->latitude) && \
            R_IS_IN_CLOSED_INTERVAL(FGeoCoordinate::minLongitude,FGeoCoordinate::maxLongitude,this->longitude) && \
            this->altitude >= FGeoCoordinate::minAltitude);
}

void FGeoCoordinate::validate() const
{
    if (!this->isValid())
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Geo location (latitude: \'%g\', longitude: \'%g\', altitude: \'%g\') is not valid.",this->latitude,this->longitude,this->altitude);
    }
}

double FGeoCoordinate::getLatitude() const
{
    return this->latitude;
}

void FGeoCoordinate::setLatitude(double latitude)
{
    R_ERROR_ASSERT(latitude >= FGeoCoordinate::minLatitude);
    R_ERROR_ASSERT(latitude <= FGeoCoordinate::maxLatitude);
    this->latitude = latitude;
}

double FGeoCoordinate::getLongitude() const
{
    return this->longitude;
}

void FGeoCoordinate::setLongitude(double longitude)
{
    R_ERROR_ASSERT(longitude >= FGeoCoordinate::minLongitude);
    R_ERROR_ASSERT(longitude <= FGeoCoordinate::maxLongitude);
    this->longitude = longitude;
}

double FGeoCoordinate::getAltitude() const
{
    return this->altitude;
}

void FGeoCoordinate::setAltitude(double altitude)
{
    R_ERROR_ASSERT(altitude >= FGeoCoordinate::minAltitude);
    this->altitude = altitude;
}

void FGeoCoordinate::merge(const FGeoCoordinate &geoCoordinates)
{
    if (this->isValid())
    {
        (*this) = geoCoordinates;
    }
}

QString FGeoCoordinate::toString() const
{
    return QString::number(this->latitude) + ","
         + QString::number(this->longitude) + ","
         + QString::number(this->altitude);
}

void FGeoCoordinate::print() const
{
    RLogger::indent();
    RLogger::info("%s: \"%g\"\n",FGeoCoordinate::ElementDesc::latitude.toUtf8().constData(),this->latitude);
    RLogger::info("%s: \"%g\"\n",FGeoCoordinate::ElementDesc::longitude.toUtf8().constData(),this->longitude);
    RLogger::info("%s: \"%g\"\n",FGeoCoordinate::ElementDesc::altitude.toUtf8().constData(),this->altitude);
    RLogger::unindent(false);
}

void FGeoCoordinate::readXmlElement(QXmlStreamReader &stream, const QString &tag)
{
    if (!stream.isStartElement() || stream.qualifiedName().toString() != tag)
    {
        stream.raiseError(QObject::tr("Missing element") + " <" + tag + ">.");
    }
    while(stream.readNextStartElement())
    {
        if (stream.qualifiedName().toString() == FGeoCoordinate::ElementName::latitude)
        {
            this->latitude = stream.readElementText().toDouble();
        }
        else if (stream.qualifiedName().toString() == FGeoCoordinate::ElementName::longitude)
        {
            this->longitude = stream.readElementText().toDouble();
        }
        else if (stream.qualifiedName().toString() == FGeoCoordinate::ElementName::altitude)
        {
            this->altitude = stream.readElementText().toDouble();
        }
        else
        {
            stream.raiseError(QObject::tr("Unexpected element") + " <" + stream.qualifiedName().toString() + ">.");
        }
    }
}

void FGeoCoordinate::writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const
{
    stream.writeStartElement(tag);
    stream.writeTextElement(FGeoCoordinate::ElementName::latitude,QString::number(this->latitude,'g',9));
    stream.writeTextElement(FGeoCoordinate::ElementName::longitude,QString::number(this->longitude,'g',9));
    stream.writeTextElement(FGeoCoordinate::ElementName::altitude,QString::number(this->altitude,'g',9));
    stream.writeEndElement();
}

FGeoCoordinate FGeoCoordinate::fromJson(const QJsonObject &json)
{
    FGeoCoordinate geoCoordinate;

    if (const QJsonValue &v = json[FGeoCoordinate::ElementName::latitude]; v.isDouble())
    {
        geoCoordinate.latitude = v.toDouble();
    }
    if (const QJsonValue &v = json[FGeoCoordinate::ElementName::longitude]; v.isDouble())
    {
        geoCoordinate.longitude = v.toDouble();
    }
    if (const QJsonValue &v = json[FGeoCoordinate::ElementName::altitude]; v.isDouble())
    {
        geoCoordinate.altitude = v.toDouble();
    }

    return geoCoordinate;
}

QJsonObject FGeoCoordinate::toJson() const
{
    QJsonObject json;

    if (this->latitude != 0.0) json[FGeoCoordinate::ElementName::latitude] = this->latitude;
    if (this->longitude != 0.0) json[FGeoCoordinate::ElementName::longitude] = this->longitude;
    if (this->altitude != 0.0) json[FGeoCoordinate::ElementName::altitude] = this->altitude;

    return json;
}

int FGeoCoordinate::compare(const FGeoCoordinate &a, const FGeoCoordinate &b, const QString &sortPath)
{
    QStringList sortPathList = sortPath.split(QLatin1Char('/'),Qt::SkipEmptyParts);
    if (sortPathList.size() > 0)
    {
        if (sortPathList.at(0) == FGeoCoordinate::ElementName::latitude)
        {
            if (std::abs(a.latitude-b.latitude) < FConstants::eps)
            {
                return 0;
            }
            else if (a.latitude < b.latitude)
            {
                return -1;
            }
            else
            {
                return 1;
            }
        }
        else if (sortPathList.at(0) == FGeoCoordinate::ElementName::longitude)
        {
            if (std::abs(a.longitude-b.longitude) < FConstants::eps)
            {
                return 0;
            }
            else if (a.longitude < b.longitude)
            {
                return -1;
            }
            else
            {
                return 1;
            }
        }
        else if (sortPathList.at(0) == FGeoCoordinate::ElementName::altitude)
        {
            if (std::abs(a.altitude-b.altitude) < FConstants::eps)
            {
                return 0;
            }
            else if (a.altitude < b.altitude)
            {
                return -1;
            }
            else
            {
                return 1;
            }
        }
    }
    return QString::compare(a.toString(),b.toString());
}
