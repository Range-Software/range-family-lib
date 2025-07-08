#include "rfl_picture.h"

const QString FPicture::ElementName::title("title");
const QString FPicture::ElementName::description("description");
const QString FPicture::ElementName::data("base64Data");

const QString FPicture::ElementDesc::title(QT_TRANSLATE_NOOP("FPicture","Title"));
const QString FPicture::ElementDesc::description(QT_TRANSLATE_NOOP("FPicture","Description"));
const QString FPicture::ElementDesc::data(QT_TRANSLATE_NOOP("FPicture","Picture binary data"));

const uint FPicture::maxWidth(1000);
const uint FPicture::maxHeight(1000);

void FPicture::_init(const FPicture *pPicture)
{
    if (pPicture)
    {
        this->title = pPicture->title;
        this->description = pPicture->description;
        this->data = pPicture->data;
    }
}

FPicture::FPicture()
{
    this->_init();
}

FPicture::FPicture(const QString &title, const QString &description, const QByteArray &base64Data)
    : title{title}
    , description{description}
    , data{base64Data}
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
            this->data == picture.data);
}

bool FPicture::isEmpty() const
{
    return this->title.isEmpty() &&
           this->description.isEmpty() &&
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
    stream.writeTextElement(FPicture::ElementName::data,this->data.toBase64());
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
    if (!this->data.isEmpty()) json[FPicture::ElementName::data] = this->data.toBase64().constData();

    return json;
}
