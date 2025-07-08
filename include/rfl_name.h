#ifndef FL_NAME_H
#define FL_NAME_H

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QJsonObject>

class FName
{

    public:

        struct ElementName
        {
            static const QString firstName;
            static const QString middleName;
            static const QString lastName;
            static const QString maidenName;
        };
        struct ElementDesc
        {
            static const QString firstName;
            static const QString middleName;
            static const QString lastName;
            static const QString maidenName;
        };

    protected:

        //! First name
        QString firstName;
        //! Middle name
        QString middleName;
        //! Last name
        QString lastName;
        //! Maiden name
        QString maidenName;

    private:

        //! Internal initialization function.
        void _init(const FName *pName = nullptr);

    public:

        //! Constructor.
        FName();

        //! Constructor.
        FName(const QString &firstName, const QString &middleName, const QString &lastName, const QString &maidenName);

        //! Copy constructor.
        FName(const FName &name);

        //! Destructor.
        ~FName();

        //! Assignment operator.
        FName &operator =(const FName &name);

        //! Equals operator.
        bool operator ==(const FName &name) const;

        //! Update with non-empty values.
        void update(const FName &name);

        //! Check if empty.
        bool isEmpty() const;

        //! Check if values are valid.
        bool isValid() const;

        //! Validate values.
        void validate() const;

        //! Get first name.
        const QString &getFirstName() const;

        //! Set first name.
        void setFirstName(const QString &firstName);

        //! Get middle name.
        const QString &getMiddleName() const;

        //! Set middle name.
        void setMiddleName(const QString &middleName);

        //! Get last name.
        const QString &getLastName() const;

        //! Set last name.
        void setLastName(const QString &lastName);

        //! Get maiden name.
        const QString &getMaidenName() const;

        //! Set maiden name.
        void setMaidenName(const QString &maidenName);

        //! Merge.
        void merge(const FName &name);

        //! Convert to single line string.
        QString toString() const;

        //! Print whole structure.
        void print() const;

        //! Read XML element.
        void readXmlElement(QXmlStreamReader &stream, const QString &tag);

        //! Write XML element.
        void writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const;

        //! Create object from Json.
        static FName fromJson(const QJsonObject &json);

        //! Create Json from object.
        QJsonObject toJson() const;

        //! Compare two objects.
        static int compare(const FName &a, const FName &b, const QString &sortPath);

};

#endif // FL_NAME_H
