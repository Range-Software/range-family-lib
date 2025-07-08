#ifndef FL_ADDRESS_H
#define FL_ADDRESS_H

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QJsonObject>

class FAddress
{

    public:

        struct ElementName
        {
            static const QString street;
            static const QString streetNumber;
            static const QString city;
            static const QString postalCode;
            static const QString country;
        };
        struct ElementDesc
        {
            static const QString street;
            static const QString streetNumber;
            static const QString city;
            static const QString postalCode;
            static const QString country;
        };

    protected:

        //! Street name
        QString street;
        //! Street number
        QString streetNumber;
        //! City
        QString city;
        //! Postal code (ZIP)
        QString postalCode;
        //! Country
        QString country;

    private:

        //! Internal initialization function.
        void _init(const FAddress *pAddress = nullptr);

    public:

        //! Constructor.
        FAddress();

        //! Constructor.
        FAddress(const QString &street, const QString &streetNumber, const QString &city, const QString &postalCode, const QString &country);

        //! Copy constructor.
        FAddress(const FAddress &address);

        //! Destructor.
        ~FAddress();

        //! Assignment operator.
        FAddress &operator =(const FAddress &address);

        //! Equals operator.
        bool operator ==(const FAddress &address) const;

        //! Update with non-empty values.
        void update(const FAddress &address);

        //! Check if empty.
        bool isEmpty() const;

        //! Check if values are valid.
        bool isValid() const;

        //! Validate values.
        void validate() const;

        //! Get street name.
        const QString &getStreet() const;

        //! Set street name.
        void setStreet(const QString &street);

        //! Get street number.
        const QString &getStreetNumber() const;

        //! Set street number.
        void setStreetNumber(const QString &streetNumber);

        //! Get city.
        const QString &getCity() const;

        //! Set city.
        void setCity(const QString &city);

        //! Get postal code.
        const QString &getPostalCode() const;

        //! Set postal code.
        void setPostalCode(const QString &postalCode);

        //! Get country.
        const QString &getCountry() const;

        //! Set country.
        void setCountry(const QString &country);

        //! Merge.
        void merge(const FAddress &address);

        //! Convert to single line string.
        QString toString() const;

        //! Print whole structure.
        void print() const;

        //! Read XML element.
        void readXmlElement(QXmlStreamReader &stream, const QString &tag);

        //! Write XML element.
        void writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const;

        //! Create object from Json.
        static FAddress fromJson(const QJsonObject &json);

        //! Create Json from object.
        QJsonObject toJson() const;

        //! Compare two objects.
        static int compare(const FAddress &a, const FAddress &b, const QString &sortPath);

};

#endif // FL_ADDRESS_H
