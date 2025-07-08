#include <rbl_logger.h>

#include "rfl_event.h"

const QString FEvent::ElementName::date("date");
const QString FEvent::ElementName::place("place");

const QString FEvent::ElementDesc::date(QT_TRANSLATE_NOOP("FEvent","Date"));
const QString FEvent::ElementDesc::place(QT_TRANSLATE_NOOP("FEvent","Place"));

void FEvent::_init(const FEvent *pEvent)
{
    if (pEvent)
    {
        this->date = pEvent->date;
        this->place = pEvent->place;
    }
}

FEvent::FEvent()
{
    this->_init();
}

FEvent::FEvent(const FDate &date, const FPlace &place)
    : date(date)
    , place(place)
{
    this->_init();
}

FEvent::FEvent(const FEvent &event)
{
    this->_init(&event);
}

FEvent::~FEvent()
{

}

FEvent &FEvent::operator =(const FEvent &event)
{
    this->_init(&event);
    return (*this);
}

bool FEvent::operator ==(const FEvent &event) const
{
    return (this->date == event.date &&
            this->place == event.place);
}

void FEvent::update(const FEvent &event)
{
    this->date.update(event.date);
    this->place.update(event.place);
}

bool FEvent::isEmpty() const
{
    return this->date.isEmpty() && this->place.isEmpty();
}

bool FEvent::isValid() const
{
    return date.isValid() && place.isValid();
}

void FEvent::validate() const
{
    date.validate();
    place.validate();
}

const FDate &FEvent::getDate() const
{
    return this->date;
}

void FEvent::setDate(const FDate &date)
{
    this->date = date;
}

const FPlace &FEvent::getPlace() const
{
    return this->place;
}

void FEvent::setPlace(const FPlace &place)
{
    this->place = place;
}

void FEvent::merge(const FEvent &event)
{
    this->date.merge(event.getDate());
    this->place.merge(event.getPlace());
}

QString FEvent::toString() const
{
    return this->date.toString() + "/" + this->place.toString();
}

void FEvent::print() const
{
    RLogger::indent();
    RLogger::info("%s:\n",FEvent::ElementDesc::date.toUtf8().constData());
    this->date.print();
    RLogger::info("%s:\n",FEvent::ElementDesc::place.toUtf8().constData());
    this->place.print();
    RLogger::unindent(false);
}

void FEvent::readXmlElement(QXmlStreamReader &stream, const QString &tag)
{
    if (!stream.isStartElement() || stream.qualifiedName().toString() != tag)
    {
        stream.raiseError(QObject::tr("Missing element") + " <" + tag + ">.");
    }
    while(stream.readNextStartElement())
    {
        if (stream.qualifiedName().toString() == FEvent::ElementName::date)
        {
            this->date.readXmlElement(stream,FEvent::ElementName::date);
        }
        else if (stream.qualifiedName().toString() == FEvent::ElementName::place)
        {
            this->place.readXmlElement(stream,FEvent::ElementName::place);
        }
        else
        {
            stream.raiseError(QObject::tr("Unexpected element") + " <" + stream.qualifiedName().toString() + ">.");
        }
    }
}

void FEvent::writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const
{
    stream.writeStartElement(tag);
    this->date.writeXmlElement(stream,FEvent::ElementName::date);
    this->place.writeXmlElement(stream,FEvent::ElementName::place);
    stream.writeEndElement();
}

FEvent FEvent::fromJson(const QJsonObject &json)
{
    FEvent event;

    if (const QJsonValue &v = json[FEvent::ElementName::date]; v.isObject())
    {
        event.date = FDate::fromJson(v.toObject());
    }
    if (const QJsonValue &v = json[FEvent::ElementName::place]; v.isObject())
    {
        event.place = FPlace::fromJson(v.toObject());
    }

    return event;
}

QJsonObject FEvent::toJson() const
{
    QJsonObject json;

    if (!this->date.isEmpty()) json[FEvent::ElementName::date] = this->date.toJson();
    if (!this->place.isEmpty()) json[FEvent::ElementName::place] = this->place.toJson();

    return json;
}

int FEvent::compare(const FEvent &a, const FEvent &b, const QString &sortPath)
{
    QStringList sortPathList = sortPath.split(QLatin1Char('/'),Qt::SkipEmptyParts);
    if (sortPathList.size() > 0)
    {
        if (sortPathList.at(0) == FEvent::ElementName::date)
        {
            sortPathList.pop_front();
            return FDate::compare(a.date,b.date,sortPathList.join(QLatin1Char('/')));
        }
        else if (sortPathList.at(0) == FEvent::ElementName::place)
        {
            sortPathList.pop_front();
            return FPlace::compare(a.place,b.place,sortPathList.join(QLatin1Char('/')));
        }
    }
    return QString::compare(a.toString(),b.toString());
}
