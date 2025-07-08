#ifndef FL_RELATION_H
#define FL_RELATION_H

#include <QUuid>
#include <QList>
#include <QXmlStreamWriter>
#include <QJsonValue>

#include "rfl_event.h"

//! Volume element group class
class FRelation
{

    public:

        struct ElementName
        {
            static const QString id;
            static const QString partners;
            static const QString children;
            static const QString partnerId;
            static const QString childId;
            static const QString begin;
            static const QString end;
        };
        struct ElementDesc
        {
            static const QString id;
            static const QString partners;
            static const QString children;
            static const QString partnerId;
            static const QString childId;
            static const QString begin;
            static const QString end;
        };

    protected:

        //! ID
        QUuid id;
        //! Partners
        QList<QUuid> partners;
        //! Children
        QList<QUuid> children;
        //! Begin of partnership
        FEvent begin;
        //! End of partnership
        FEvent end;

    private:

        //! Internal initialization function.
        void _init(const FRelation *pRelation = nullptr);

    public:

        //! Constructor.
        FRelation();

        //! Copy constructor.
        FRelation(const FRelation &relation);

        //! Destructor.
        ~FRelation();

        //! Assignment operator.
        FRelation & operator =(const FRelation &relation);

        //! Equals operator.
        bool operator ==(const FRelation &relation) const;

        //! Not-equals operator.
        bool operator !=(const FRelation &relation) const;

        //! Update with non-empty values.
        void update(const FRelation &relation);

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

        //! Get partners.
        const QList<QUuid> &getPartners() const;

        //! Get partners.
        QList<QUuid> &getPartners();

        //! Set partners.
        void setPartners(const QList<QUuid> &partners);

        //! Get children.
        const QList<QUuid> &getChildren() const;

        //! Get children.
        QList<QUuid> &getChildren();

        //! Set children.
        void setChildren(const QList<QUuid> &children);

        //! Get begin event.
        const FEvent &getBegin() const;

        //! Set begin event.
        void setBegin(const FEvent &begin);

        //! Get end event.
        const FEvent &getEnd() const;

        //! Set begin event.
        void setEnd(const FEvent &end);

        //! Get partners as string.
        QString getPartnersString() const;

        //! Get children as string.
        QString getChildrenString() const;

        //! Merge.
        void merge(const FRelation &relation);

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
        static FRelation fromJson(const QJsonObject &json);

        //! Create Json from object.
        QJsonObject toJson() const;

        //! Compare two objects.
        static int compare(const FRelation &a, const FRelation &b, const QString &sortPath);

    private:

        //! Compare if two uuid lists contain same ids.
        static bool compareListContents(const QList<QUuid> &a, const QList<QUuid> &b);

};

#endif /* FL_RELATION_H */
