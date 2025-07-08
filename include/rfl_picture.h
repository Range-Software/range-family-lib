#ifndef FL_PICTURE_H
#define FL_PICTURE_H

#include <QString>
#include <QByteArray>
#include <QXmlStreamReader>
#include <QJsonObject>

class FPicture
{

    public:

        struct ElementName
        {
            static const QString title;
            static const QString description;
            static const QString data;
        };
        struct ElementDesc
        {
            static const QString title;
            static const QString description;
            static const QString data;
        };
        static const uint maxWidth;
        static const uint maxHeight;

    protected:

        //! Picture title.
        QString title;
        //! Picture description.
        QString description;
        //! Picture data.
        QByteArray data;

    private:

        //! Internal initialization function.
        void _init(const FPicture *pPicture = nullptr);

    public:

        //! Constructor.
        FPicture();

        //! Full constructor.
        FPicture(const QString &title, const QString &description, const QByteArray &base64Data);

        //! Copy constructor.
        FPicture(const FPicture &picture);

        //! Destructor.
        ~FPicture();

        //! Assignment operator.
        FPicture &operator =(const FPicture &picture);

        //! Equals operator.
        bool operator ==(const FPicture &picture) const;

        //! Check if empty.
        bool isEmpty() const;

        //! Return const reference to title.
        const QString &getTitle() const;

        //! Set new title.
        void setTitle(const QString &title);

        //! Return const reference to description.
        const QString &getDescription() const;

        //! Set new description.
        void setDescription(const QString &description);

        //! Return const reference to picture data.
        const QByteArray &getData() const;

        //! Set new picture data.
        void setData(const QByteArray &base64Data);

        //! Read XML element.
        void readXmlElement(QXmlStreamReader &stream, const QString &tag);

        //! Write XML element.
        void writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const;

        //! Create object from Json.
        static FPicture fromJson(const QJsonObject &json);

        //! Create Json from object.
        QJsonObject toJson() const;

};

#endif // FL_PICTURE_H
