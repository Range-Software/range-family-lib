#include <QObject>

#include <rbl_logger.h>
#include <rbl_error.h>

#include "rfl_date.h"

const QString FDate::ElementName::fuzzy("fuzzy");
const QString FDate::ElementName::day("day");
const QString FDate::ElementName::month("month");
const QString FDate::ElementName::year("year");

const QString FDate::ElementDesc::fuzzy(QT_TRANSLATE_NOOP("FDate","Fuzzy"));
const QString FDate::ElementDesc::day(QT_TRANSLATE_NOOP("FDate","Day"));
const QString FDate::ElementDesc::month(QT_TRANSLATE_NOOP("FDate","Month"));
const QString FDate::ElementDesc::year(QT_TRANSLATE_NOOP("FDate","Year"));

void FDate::_init(const FDate *pDate)
{
    if (pDate)
    {
        this->fuzzy = pDate->fuzzy;
        this->day = pDate->day;
        this->month = pDate->month;
        this->year = pDate->year;
    }
}

FDate::FDate()
    : fuzzy(false)
    , day(0)
    , month(0)
    , year(INT_MIN)
{
    this->_init();
}

FDate::FDate(int day, int month, int year, bool fuzzy)
    : fuzzy(fuzzy)
    , day(day)
    , month(month)
    , year(year)
{
    this->_init();
}

FDate::FDate(const QDate &date, bool fuzzy)
    : fuzzy(fuzzy)
    , day(date.day())
    , month(date.month())
    , year(date.year())
{
    this->_init();
}

FDate::FDate(const FDate &date)
{
    this->_init(&date);
}

FDate::~FDate()
{

}

FDate &FDate::operator =(const FDate &date)
{
    this->_init(&date);
    return (*this);
}

bool FDate::operator ==(const FDate &date) const
{
    return (this->fuzzy == date.fuzzy &&
            this->day == date.day &&
            this->month == date.month &&
            this->year == date.year);
}

void FDate::update(const FDate &date)
{
    if (date.day != 0)
    {
        this->day = date.day;
    }
    if (date.month != 0)
    {
        this->month = date.month;
    }
    if (date.year != INT_MIN)
    {
        this->year = date.year;
    }
}

bool FDate::isEmpty() const
{
    return (this->day == 0 && this->month == 0 && this->year == INT_MIN);
}

void FDate::empty(bool fuzzy)
{
    this->day = 0;
    this->month = 0;
    this->year = INT_MIN;
    this->fuzzy = fuzzy;
}

bool FDate::isValid() const
{
    return this->isEmpty() || QDate::isValid(this->year,this->month,this->day);
}

void FDate::validate() const
{
    if (!this->isValid())
    {
        throw RError(RError::Type::InvalidInput,R_ERROR_REF,"Date (year: \'%d\', month: \'%d\', day: \'%d\') is not valid.",this->year,this->month,this->day);
    }
}

bool FDate::getFuzzy() const
{
    return this->fuzzy;
}

void FDate::setFuzzy(bool fuzzy)
{
    this->fuzzy = fuzzy;
}

int FDate::getDay() const
{
    return this->day;
}

void FDate::setDay(int day)
{
    this->day = day;
}

int FDate::getMonth() const
{
    return this->month;
}

void FDate::setMonth(int month)
{
    this->month = month;
}

int FDate::getYear() const
{
    return this->year;
}

void FDate::setYear(int year)
{
    this->year = year;
}

void FDate::merge(const FDate &date)
{
    if (this->day == 0 || this->month == 0 || this->year == INT_MIN)
    {
        (*this) = date;
    }
}

uint FDate::calculateAge() const
{
    return uint(this->toDate().daysTo(QDate::currentDate()));
}

QDate FDate::toDate() const
{
    return QDate(this->year,this->month,this->day);
}

QString FDate::toString() const
{
    return this->toDate().toString("dd.MM.yyyy") + (this->fuzzy ? " (" + QObject::tr("fuzzy") + ")" : "");
}

void FDate::print() const
{
    RLogger::indent();
    RLogger::info("%s: \"%s\"\n",FDate::ElementDesc::fuzzy.toUtf8().constData(),this->fuzzy?"true":"false");
    RLogger::info("%s: \"%d\"\n",FDate::ElementDesc::day.toUtf8().constData(),this->day);
    RLogger::info("%s: \"%d\"\n",FDate::ElementDesc::month.toUtf8().constData(),this->month);
    RLogger::info("%s: \"%d\"\n",FDate::ElementDesc::year.toUtf8().constData(),this->year);
    RLogger::unindent(false);
}

void FDate::readXmlElement(QXmlStreamReader &stream, const QString &tag)
{
    if (!stream.isStartElement() || stream.qualifiedName().toString() != tag)
    {
        stream.raiseError(QObject::tr("Missing element") + " <" + tag + ">.");
    }
    while(stream.readNextStartElement())
    {
        if (stream.qualifiedName().toString() == FDate::ElementName::fuzzy)
        {
            this->fuzzy = bool(stream.readElementText().toInt());
        }
        else if (stream.qualifiedName().toString() == FDate::ElementName::day)
        {
            this->day = stream.readElementText().toInt();
        }
        else if (stream.qualifiedName().toString() == FDate::ElementName::month)
        {
            this->month = stream.readElementText().toInt();
        }
        else if (stream.qualifiedName().toString() == FDate::ElementName::year)
        {
            this->year = stream.readElementText().toInt();
        }
        else
        {
            stream.raiseError(QObject::tr("Unexpected element") + " <" + stream.qualifiedName().toString() + ">.");
        }
    }
}

void FDate::writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const
{
    stream.writeStartElement(tag);
    stream.writeTextElement(FDate::ElementName::fuzzy,QString::number(this->fuzzy));
    stream.writeTextElement(FDate::ElementName::day,QString::number(this->day));
    stream.writeTextElement(FDate::ElementName::month,QString::number(this->month));
    stream.writeTextElement(FDate::ElementName::year,QString::number(this->year));
    stream.writeEndElement();
}

FDate FDate::fromJson(const QJsonObject &json)
{
    FDate date;

    if (const QJsonValue &v = json[FDate::ElementName::fuzzy]; v.isBool())
    {
        date.fuzzy = v.toBool();
    }
    if (const QJsonValue &v = json[FDate::ElementName::day]; v.isDouble())
    {
        date.day = v.toInt();
    }
    if (const QJsonValue &v = json[FDate::ElementName::month]; v.isDouble())
    {
        date.month = v.toInt();
    }
    if (const QJsonValue &v = json[FDate::ElementName::year]; v.isDouble())
    {
        date.year = v.toInt();
    }

    return date;
}

QJsonObject FDate::toJson() const
{
    QJsonObject json;

    json[FDate::ElementName::fuzzy] = this->fuzzy;
    if (this->day != 0) json[FDate::ElementName::day] = this->day;
    if (this->month != 0) json[FDate::ElementName::month] = this->month;
    if (this->year != INT_MIN) json[FDate::ElementName::year] = this->year;

    return json;
}

int FDate::compare(const FDate &a, const FDate &b, const QString &sortPath)
{
    QStringList sortPathList = sortPath.split(QLatin1Char('/'),Qt::SkipEmptyParts);
    if (sortPathList.size() > 0)
    {
        if (sortPathList.at(0) == FDate::ElementName::day)
        {
            return a.day-b.day;
        }
        else if (sortPathList.at(0) == FDate::ElementName::month)
        {
            return a.month-b.month;
        }
        else if (sortPathList.at(0) == FDate::ElementName::year)
        {
            return a.year-b.year;
        }
    }
    QDate aDate(a.toDate());
    QDate bDate(b.toDate());
    if (aDate < bDate)
    {
        return -1;
    }
    if (aDate > bDate)
    {
        return 1;
    }
    return 0;
}

FDate FDate::findMiddle(const FDate &a, const FDate &b)
{
    if (a.isEmpty() && b.isEmpty())
    {
        return FDate();
    }
    if (!a.isEmpty() && b.isEmpty())
    {
        return a;
    }
    if (a.isEmpty() && !b.isEmpty())
    {
        return b;
    }
    QDate aDate(a.toDate());
    QDate bDate(b.toDate());
    return FDate(aDate.addDays(qint64(std::round(double(aDate.daysTo(bDate))/2.0))),a.getFuzzy() || b.getFuzzy());
}
