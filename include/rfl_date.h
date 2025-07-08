#ifndef FL_DATE_H
#define FL_DATE_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDate>
#include <QJsonObject>

class FDate
{

    public:

        struct ElementName
        {
            static const QString fuzzy;
            static const QString day;
            static const QString month;
            static const QString year;
        };
        struct ElementDesc
        {
            static const QString fuzzy;
            static const QString day;
            static const QString month;
            static const QString year;
        };

    protected:

        //! Fuzzy flag
        bool fuzzy;
        //! Day
        int day;
        //! Month
        int month;
        //! Year
        int year;

    private:

        //! Internal initialization function.
        void _init(const FDate *pDate = nullptr);

    public:

        //! Constructor.
        FDate();

        //! Constructor.
        FDate(int day, int month, int year, bool fuzzy = false);

        //! Constructor.
        FDate(const QDate &date, bool fuzzy = false);

        //! Copy constructor.
        FDate(const FDate &date);

        //! Destructor.
        ~FDate();

        //! Assignment operator.
        FDate &operator =(const FDate &date);

        //! Equals operator.
        bool operator ==(const FDate &date) const;

        //! Update with non-empty values.
        void update(const FDate &date);

        //! Check if empty.
        bool isEmpty() const;

        //! Empty all values.
        void empty(bool fuzzy = false);

        //! Check if values are valid.
        bool isValid() const;

        //! Validate values.
        void validate() const;

        //! Get fuzzy.
        bool getFuzzy() const;

        //! Set fuzzy.
        void setFuzzy(bool fuzzy);

        //! Get day name.
        int getDay() const;

        //! Set day name.
        void setDay(int day);

        //! Get month.
        int getMonth() const;

        //! Set month.
        void setMonth(int month);

        //! Get year.
        int getYear() const;

        //! Set year.
        void setYear(int year);

        //! Merge.
        void merge(const FDate &date);

        //! Calculate age in days.
        uint calculateAge() const;

        //! Convert to QDate.
        QDate toDate() const;

        //! Convert to single line string.
        QString toString() const;

        //! Print whole structure.
        void print() const;

        //! Read XML element.
        void readXmlElement(QXmlStreamReader &stream, const QString &tag);

        //! Write XML element.
        void writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const;

        //! Create object from Json.
        static FDate fromJson(const QJsonObject &json);

        //! Create Json from object.
        QJsonObject toJson() const;

        //! Compare two objects.
        static int compare(const FDate &a, const FDate &b, const QString &sortPath);

        //! Return middle date.
        static FDate findMiddle(const FDate &a, const FDate &b);

};

#endif // FL_DATE_H
