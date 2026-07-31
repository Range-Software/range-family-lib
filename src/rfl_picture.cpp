#include <QDir>
#include <QFile>
#include <QUrl>

#include <rbl_logger.h>

#include "rfl_picture.h"

const QString FPicture::ElementName::title("title");
const QString FPicture::ElementName::description("description");
const QString FPicture::ElementName::url("url");
const QString FPicture::ElementName::data("base64Data");

const QString FPicture::ElementDesc::title(QT_TRANSLATE_NOOP("FPicture","Title"));
const QString FPicture::ElementDesc::description(QT_TRANSLATE_NOOP("FPicture","Description"));
const QString FPicture::ElementDesc::url(QT_TRANSLATE_NOOP("FPicture","Picture url"));
const QString FPicture::ElementDesc::data(QT_TRANSLATE_NOOP("FPicture","Picture binary data"));

const uint FPicture::maxWidth(1000);
const uint FPicture::maxHeight(1000);

void FPicture::_init(const FPicture *pPicture)
{
    if (pPicture)
    {
        this->title = pPicture->title;
        this->description = pPicture->description;
        this->url = pPicture->url;
        this->data = pPicture->data;
    }
}

QString FPicture::toLocalFilePath(const QString &url)
{
    if (url.isEmpty())
    {
        return QString();
    }

    // A relative url is meaningless without a base directory. Resolving it
    // against the current working directory would silently pick the wrong file.
    if (!QDir::isAbsolutePath(url) && !url.startsWith("file:",Qt::CaseInsensitive))
    {
        RLogger::warning("Picture url \"%s\" is not an absolute path.\n",url.toUtf8().constData());
        return QString();
    }

    QUrl pictureUrl(QUrl::fromUserInput(url,QString(),QUrl::AssumeLocalFile));

    if (!pictureUrl.isValid() || !pictureUrl.isLocalFile())
    {
        RLogger::warning("Picture url \"%s\" does not point to a local file.\n",url.toUtf8().constData());
        return QString();
    }

    return pictureUrl.toLocalFile();
}

QByteArray FPicture::readLocalFileData(const QString &url)
{
    QString fileName(FPicture::toLocalFilePath(url));

    if (fileName.isEmpty())
    {
        return QByteArray();
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        RLogger::warning("Failed to open picture file \"%s\". %s\n",
                         fileName.toUtf8().constData(),
                         file.errorString().toUtf8().constData());
        return QByteArray();
    }

    QByteArray fileData(file.readAll());
    file.close();

    if (fileData.isEmpty())
    {
        RLogger::warning("Picture file \"%s\" is empty.\n",fileName.toUtf8().constData());
        return QByteArray();
    }

    RLogger::debug("Picture data were read from file \"%s\".\n",fileName.toUtf8().constData());

    return fileData;
}

bool FPicture::writeLocalFileData(const QString &url, const QByteArray &data)
{
    if (data.isEmpty())
    {
        return false;
    }

    QString fileName(FPicture::toLocalFilePath(url));

    if (fileName.isEmpty())
    {
        return false;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        RLogger::warning("Failed to open picture file \"%s\" for writing. %s\n",
                         fileName.toUtf8().constData(),
                         file.errorString().toUtf8().constData());
        return false;
    }

    if (file.write(data) != data.size())
    {
        RLogger::warning("Failed to write picture file \"%s\". %s\n",
                         fileName.toUtf8().constData(),
                         file.errorString().toUtf8().constData());
        file.close();
        return false;
    }

    file.close();

    RLogger::debug("Picture data were written to file \"%s\".\n",fileName.toUtf8().constData());

    return true;
}

FPicture::FPicture()
{
    this->_init();
}

FPicture::FPicture(const QString &title, const QString &description, const QString &url, const QByteArray &data)
    : title{title}
    , description{description}
    , url{url}
    , data{data}
{
    this->_init();
}

FPicture::FPicture(const FPicture &picture)
{
    this->_init(&picture);
}

FPicture::~FPicture()
{

}

FPicture &FPicture::operator =(const FPicture &picture)
{
    this->_init(&picture);
    return (*this);
}

bool FPicture::operator ==(const FPicture &picture) const
{
    return (this->title == picture.title &&
            this->description == picture.description &&
            this->url == picture.url &&
            this->data == picture.data);
}

bool FPicture::isEmpty() const
{
    return this->title.isEmpty() &&
           this->description.isEmpty() &&
           this->url.isEmpty() &&
           this->data.isEmpty();
}

const QString &FPicture::getTitle() const
{
    return this->title;
}

void FPicture::setTitle(const QString &title)
{
    this->title = title;
}

const QString &FPicture::getDescription() const
{
    return this->description;
}

void FPicture::setDescription(const QString &description)
{
    this->description = description;
}

const QString &FPicture::getUrl() const
{
    return this->url;
}

void FPicture::setUrl(const QString &url)
{
    this->url = url;
}

const QByteArray &FPicture::getData() const
{
    return this->data;
}

void FPicture::setData(const QByteArray &base64Data)
{
    this->data = base64Data;
}

void FPicture::readXmlElement(QXmlStreamReader &stream, const QString &tag)
{
    if (!stream.isStartElement() || stream.qualifiedName().toString() != tag)
    {
        stream.raiseError(QObject::tr("Missing element") + " <" + tag + ">.");
    }
    while(stream.readNextStartElement())
    {
        if (stream.qualifiedName().toString() == FPicture::ElementName::title)
        {
            this->title = stream.readElementText();
        }
        else if (stream.qualifiedName().toString() == FPicture::ElementName::description)
        {
            this->description = stream.readElementText();
        }
        else if (stream.qualifiedName().toString() == FPicture::ElementName::url)
        {
            this->url = stream.readElementText();
        }
        else if (stream.qualifiedName().toString() == FPicture::ElementName::data)
        {
            this->data = QByteArray::fromBase64(stream.readElementText().toUtf8());
        }
        else
        {
            stream.raiseError(QObject::tr("Unexpected element") + " <" + stream.qualifiedName().toString() + ">.");
        }
    }
}

void FPicture::writeXmlElement(QXmlStreamWriter &stream, const QString &tag) const
{
    stream.writeStartElement(tag);
    stream.writeTextElement(FPicture::ElementName::title,this->title);
    stream.writeTextElement(FPicture::ElementName::description,this->description);
    stream.writeTextElement(FPicture::ElementName::url,this->url);
    // Data stored in a local file referenced by the url are not embedded.
    if (!this->data.isEmpty())
    {
        stream.writeTextElement(FPicture::ElementName::data,this->data.toBase64());
    }
    stream.writeEndElement(); // picture
}

FPicture FPicture::fromJson(const QJsonObject &json)
{
    FPicture picture;

    if (const QJsonValue &v = json[FPicture::ElementName::title]; v.isString())
    {
        picture.title = v.toString();
    }
    if (const QJsonValue &v = json[FPicture::ElementName::description]; v.isString())
    {
        picture.description = v.toString();
    }
    if (const QJsonValue &v = json[FPicture::ElementName::url]; v.isString())
    {
        picture.url = v.toString();
    }
    if (const QJsonValue &v = json[FPicture::ElementName::data]; v.isString())
    {
        picture.data = QByteArray::fromBase64(v.toString().toUtf8());
    }

    return picture;
}

QJsonObject FPicture::toJson() const
{
    QJsonObject json;

    if (!this->title.isEmpty()) json[FPicture::ElementName::title] = this->title;
    if (!this->description.isEmpty()) json[FPicture::ElementName::description] = this->description;
    if (!this->url.isEmpty()) json[FPicture::ElementName::url] = this->url;
    // Data stored in a local file referenced by the url are not embedded.
    if (!this->data.isEmpty()) json[FPicture::ElementName::data] = this->data.toBase64().constData();

    return json;
}
