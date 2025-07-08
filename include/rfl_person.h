#ifndef FL_PERSON_H
#define FL_PERSON_H

#include <QString>
#include <QUuid>
#include <QXmlStreamWriter>
#include <QJsonObject>

#include "rfl_name.h"
#include "rfl_event.h"
#include "rfl_picture.h"

class FPerson
{

    public:

        struct ElementName
        {
            static const QString id;
            static const QString sex;
            static const QString name;
            static const QString birth;
            static const QString death;
            static const QString text;
            static const QString picture;
        };
        struct ElementDesc
        {
            static const QString id;
            static const QString sex;
            static const QString name;
            static const QString birth;
            static const QString death;
            static const QString text;
            static const QString picture;
        };
        enum SexIndex
        {
            undefined = 0,
            male,
            female,
            other,
            nSexes
        };
        struct SexDesc
        {
            static const QString male;
            static const QString female;
            static const QString other;
        };

    protected:

        //! ID
        QUuid id;
        //! Sex
        QString sex;
        //! Name
        FName name;
        //! Birth
        FEvent birth;
        //! Death
        FEvent death;
        //! Additional information
        QString text;
        //! Picture.
        FPicture picture;

    private:

        //! Internal initialization function.
        void _init(const FPerson *pPerson = nullptr);

    public:

        //! Constructor.
        FPerson();

        //! Full constructor.
        FPerson(const QUuid &id, const QString &sex, const FName &name, const FEvent &birth, const FEvent &death, const QString &text, const FPicture &picture);

        //! Copy constructor.
        FPerson(const FPerson &person);

        //! Destructor.
        ~FPerson();

        //! Assignment operator.
        FPerson &operator =(const FPerson &person);

        //! Equals operator.
        bool operator ==(const FPerson &person) const;

        //! Not-equals operator.
        bool operator !=(const FPerson &person) const;

        //! Update with non-empty values.
        void update(const FPerson &person);

        //! Check if empty.
        bool isEmpty() const;

        //! Check if values are valid.
        bool isValid() const;

        //! Validate values.
        void validate() const;

        //! Get ID.
        const QUuid &getId() const;

        //! Set ID.
        void setId(const QUuid &id);

        //! Get sex.
        const QString &getSex() const;

        //! Set sex.
        void setSex(const QString &sex);

        //! Get name.
        const FName &getName() const;

        //! Set name.
        void setName(const FName &name);

        //! Get birth.
        const FEvent &getBirth() const;

        //! Set birth.
        void setBirth(const FEvent &birth);

        //! Get death.
        const FEvent &getDeath() const;

        //! Set death.
        void setDeath(const FEvent &death);

        //! Get text.
        const QString &getText() const;

        //! Set text.
        void setText(const QString &text);

        //! Get picture.
        const FPicture &getPicture() const;

        //! Set picture.
        void setPicture(const FPicture &picture);

        //! Merge.
        void merge(const FPerson &person);

        //! Convert to single line string.
        QString toString() const;

        //! Convert to human readable short single line string.
        QString toShortString() const;

        //! Print whole structure.
        void print() const;

        //! Read XML element.
        void readXmlElement(QXmlStreamReader &stream, const QString &tag);

        //! Write XML element.
        void writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const;

        //! Create object from Json.
        static FPerson fromJson(const QJsonObject &json);

        //! Create Json from object.
        QJsonObject toJson() const;

        //! Compare two objects.
        static int compare(const FPerson &a, const FPerson &b, const QString &sortPath);

        //! Convert sex description to index.
        static int sexDescToIndex(const QString &sexDesc);

        //! Convert sex index to description.
        static QString sexIndexToDesc(int sexIndex);

        //! Convert sex index to key.
        static QString sexIndexToKey(int sexIndex);

        //! Convert sex description to key.
        static QString sexDescToKey(const QString &sexDesc);

        //! Convert sex key to description.
        static QString sexKeyToDesc(const QString &sexKey);

};

#endif /* FL_PERSON_H */
