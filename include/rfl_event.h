#ifndef FL_EVENT_H
#define FL_EVENT_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QJsonValue>

#include "rfl_date.h"
#include "rfl_place.h"

class FEvent
{

    public:

        struct ElementName
        {
            static const QString date;
            static const QString place;
        };
        struct ElementDesc
        {
            static const QString date;
            static const QString place;
        };

    protected:

        //! Date
        FDate date;
        //! Place
        FPlace place;

    private:

        //! Internal initialization function.
        void _init(const FEvent *pEvent = nullptr);

    public:

        //! Constructor.
        FEvent();

        //! Constructor.
        FEvent(const FDate &date, const FPlace &place);

        //! Copy constructor.
        FEvent(const FEvent &event);

        //! Destructor.
        ~FEvent();

        //! Assignment operator.
        FEvent &operator =(const FEvent &event);

        //! Equals operator.
        bool operator ==(const FEvent &event) const;

        //! Update with non-empty values.
        void update(const FEvent &event);

        //! Check if empty.
        bool isEmpty() const;

        //! Check if values are valid.
        bool isValid() const;

        //! Validate values.
        void validate() const;

        //! Get date name.
        const FDate &getDate() const;

        //! Set date name.
        void setDate(const FDate &date);

        //! Get place.
        const FPlace &getPlace() const;

        //! Set place.
        void setPlace(const FPlace &place);

        //! Merge.
        void merge(const FEvent &event);

        //! Convert to single line string.
        QString toString() const;

        //! Print whole structure.
        void print() const;

        //! Read XML element.
        void readXmlElement(QXmlStreamReader &stream, const QString &tag);

        //! Write XML element.
        void writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const;

        //! Create object from Json.
        static FEvent fromJson(const QJsonObject &json);

        //! Create Json from object.
        QJsonObject toJson() const;

        //! Compare two objects.
        static int compare(const FEvent &a, const FEvent &b, const QString &sortPath);

};

#endif // FL_EVENT_H
