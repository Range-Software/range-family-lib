#ifndef FL_PLACE_H
#define FL_PLACE_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QJsonObject>

#include "rfl_address.h"
#include "rfl_geo_coordinate.h"

class FPlace
{

    public:

        struct ElementName
        {
            static const QString address;
            static const QString geoCoordinate;
        };
        struct ElementDesc
        {
            static const QString address;
            static const QString geoCoordinate;
        };

    protected:

        //! Address
        FAddress address;
        //! Geographic coordinates
        FGeoCoordinate geoCoordinate;

    private:

        //! Internal initialization function.
        void _init(const FPlace *pPlace = nullptr);

    public:

        //! Constructor.
        FPlace();

        //! Constructor.
        FPlace(const FAddress &address, const FGeoCoordinate &geoCoordinates);

        //! Copy constructor.
        FPlace(const FPlace &place);

        //! Destructor.
        ~FPlace();

        //! Assignment operator.
        FPlace &operator =(const FPlace &place);

        //! Equals operator.
        bool operator ==(const FPlace &place) const;

        //! Update with non-empty values.
        void update(const FPlace &place);

        //! Check if empty.
        bool isEmpty() const;

        //! Check if values are valid.
        bool isValid() const;

        //! Validate values.
        void validate() const;

        //! Get address name.
        const FAddress &getAddress() const;

        //! Set address name.
        void setAddress(const FAddress &address);

        //! Get geoCoordinates.
        const FGeoCoordinate &getGeoCoordinates() const;

        //! Set geoCoordinates.
        void setGeoCoordinates(const FGeoCoordinate &geoCoordinates);

        //! Merge.
        void merge(const FPlace &place);

        //! Convert to single line string.
        QString toString() const;

        //! Print whole structure.
        void print() const;

        //! Read XML element.
        void readXmlElement(QXmlStreamReader &stream, const QString &tag);

        //! Write XML element.
        void writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const;

        //! Create object from Json.
        static FPlace fromJson(const QJsonObject &json);

        //! Create Json from object.
        QJsonObject toJson() const;

        //! Compare two objects.
        static int compare(const FPlace &a, const FPlace &b, const QString &sortPath);

};

#endif // FL_PLACE_H
