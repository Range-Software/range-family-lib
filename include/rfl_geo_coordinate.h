#ifndef FL_GEO_COORDINATE_H
#define FL_GEO_COORDINATE_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QJsonObject>

class FGeoCoordinate
{

    public:

        struct ElementName
        {
            static const QString latitude;
            static const QString longitude;
            static const QString altitude;
        };
        struct ElementDesc
        {
            static const QString latitude;
            static const QString longitude;
            static const QString altitude;
        };

    public:

        //! Limits
        static const double minLatitude;
        static const double maxLatitude;
        static const double minLongitude;
        static const double maxLongitude;
        static const double minAltitude;

    protected:

        //! Latitude
        double latitude;
        //! Longitude
        double longitude;
        //! Altitude
        double altitude;

    private:

        //! Internal initialization function.
        void _init(const FGeoCoordinate *pGeoCoordinates = nullptr);

    public:

        //! Constructor.
        FGeoCoordinate();

        //! Constructor.
        FGeoCoordinate(double latitude, double longitude, double altitude);

        //! Copy constructor.
        FGeoCoordinate(const FGeoCoordinate &geoCoordinates);

        //! Destructor.
        ~FGeoCoordinate();

        //! Assignment operator.
        FGeoCoordinate &operator =(const FGeoCoordinate &geoCoordinates);

        //! Equals operator.
        bool operator ==(const FGeoCoordinate &geoCoordinates) const;

        //! Update with non-empty values.
        void update(const FGeoCoordinate &geoCoordinates);

        //! Check if empty.
        bool isEmpty() const;

        //! Check if values are valid.
        bool isValid() const;

        //! Validate values.
        void validate() const;

        //! Get latitude name.
        double getLatitude() const;

        //! Set latitude name.
        void setLatitude(double latitude);

        //! Get longitude.
        double getLongitude() const;

        //! Set longitude.
        void setLongitude(double longitude);

        //! Get altitude.
        double getAltitude() const;

        //! Set altitude.
        void setAltitude(double altitude);

        //! Merge.
        void merge(const FGeoCoordinate &geoCoordinates);

        //! Convert to single line string.
        QString toString() const;

        //! Print whole structure.
        void print() const;

        //! Read XML element.
        void readXmlElement(QXmlStreamReader &stream, const QString &tag);

        //! Write XML element.
        void writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const;

        //! Create object from Json.
        static FGeoCoordinate fromJson(const QJsonObject &json);

        //! Create Json from object.
        QJsonObject toJson() const;

        //! Compare two objects.
        static int compare(const FGeoCoordinate &a, const FGeoCoordinate &b, const QString &sortPath);

};

#endif // FL_GEO_COORDINATE_H
