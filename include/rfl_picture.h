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
            static const QString url;
            static const QString data;
        };
        struct ElementDesc
        {
            static const QString title;
            static const QString description;
            static const QString url;
            static const QString data;
        };
        static const uint maxWidth;
        static const uint maxHeight;

    protected:

        //! Picture title.
        QString title;
        //! Picture description.
        QString description;
        //! Picture url.
        QString url;
        //! Picture data.
        QByteArray data;

    private:

        //! Internal initialization function.
        void _init(const FPicture *pPicture = nullptr);

        //! Convert given url to a local file path.
        //! Return empty string if url is empty, relative or does not point to
        //! a local file. Relative urls are rejected because they can only be
        //! resolved against the family tree file, which FPicture does not know.
        static QString toLocalFilePath(const QString &url);

    public:

        //! Read picture content from a local file referenced by given url.
        //! Url must be absolute. Return empty byte array if the file can not be read.
        static QByteArray readLocalFileData(const QString &url);

        //! Write picture content to a local file referenced by given url.
        //! Url must be absolute. Return false if the file can not be written.
        static bool writeLocalFileData(const QString &url, const QByteArray &data);

        //! Constructor.
        FPicture();

        //! Full constructor.
        FPicture(const QString &title, const QString &description, const QString &url, const QByteArray &data);

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

        //! Return const reference to url.
        const QString &getUrl() const;

        //! Set new url.
        void setUrl(const QString &url);

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
