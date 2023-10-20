
#include "Book.h"

#include <QUrl>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QJsonDocument>
#include <QDomDocument>

#include "tidy.h"
#include "tidybuffio.h"
#include "tidyenum.h"
#include "tidyplatform.h"
#include <iostream>
#include <fstream>
#include <string>



bool isNodeEmpty(const QDomNode& node) {
    if (node.isText()) {
        QString text = node.toText().data();
        return text.trimmed().isEmpty();
    }

    if (node.isElement()) {
        // Check if the element is empty by iterating through its children
        QDomNode child = node.firstChild();
        while (!child.isNull()) {
            if (!isNodeEmpty(child)) {
                return false; // The element has non-empty content
            }
            child = child.nextSibling();
        }
    }

    return true; // Node is empty
}

QString findTextValue(const QDomNode& node) {

    QString retVal = "";
    QDomNode child = node.firstChild();


    while (!child.isNull())
    {
        if (child.isElement())
        {
            return findTextValue(child).trimmed(); // Recurse into child elements
        }
        else if (child.isText())
        {
            QDomText textNode = child.toText();
            QString textValue = textNode.data();
            if (!textValue.trimmed().isEmpty())
            {
                return textValue.trimmed();

            }
        }
        child = child.nextSibling();
    }

    return retVal.trimmed();
}

QString findImageSRC(const QDomNode& node)
{
    QDomElement imgElement = node.firstChildElement("img");

    if (!imgElement.isNull())
    {
        // Get the value of the src attribute
        return imgElement.attribute("src");

    } else {
        qDebug() << "No <img> tag found inside the <td>.";
    }
    return "";
}

Index::Index()
{
    title = "";
    author = "";
}

bool Index::fromJson(const QJsonObject &obj)
{
    title = obj["title"].toString();
    author = obj["author"].toString();

    return true;
}

QJsonObject Index::toJson()
{
    QJsonObject obj;

    obj["title"] = title;
    obj["author"] = author;

    return obj;

}




Book::Book()
{
    title_ita = "";
    title_orig = "";
    number = 0;
    author = "";
    date_pub = QDate::fromString("02/08/1969", "yyyy-MM-dd");
    cover_author = "N/A";
    cover_image = QByteArray();
    synopsis = "";
    owned = false;
    stars = 0;
    comment = "";
    reprint = false;
    read = false;
}

bool Book::fromJson(const QJsonObject &obj)
{

    title_ita = obj["title_ita"].toString();
    title_orig = obj["title_orig"].toString();
    author = obj["author"].toString();
    date_pub = QDate::fromString(obj["date_pub"].toString(), "yyyy-MM-dd");
    cover_author = obj["cover_author"].toString();
    cover_image = QByteArray::fromBase64( obj["cover_image"].toString().toLatin1() );
    synopsis = obj["synopsis"].toString();
    synopsis_image = QByteArray::fromBase64( obj["synopsis_image"].toString().toLatin1() );

    owned = obj["owned"].toBool();
    number = obj["number"].toInt();
    stars = obj["stars"].toInt();
    comment = obj["comment"].toString();
    reprint = obj["reprint"].toBool();
    read = obj["read"].toBool();



    QJsonArray indexArray = obj["index"].toArray();
    for (int i = 0; i < indexArray.size(); ++i) {
        QJsonObject indexObject = indexArray[i].toObject();
        Index index;
        index.fromJson(indexObject);
        this->index.append(index);
    }

    return true;
}

QJsonObject Book::toJson()
{
    QJsonObject obj;

    obj["title_ita"] = title_ita;
    obj["title_orig"] = title_orig;
    obj["author"] = author;
    obj["date_pub"] = date_pub.toString("yyyy-MM-dd");
    obj["cover_author"] = cover_author;
    obj["cover_image"] = QString( cover_image.toBase64() );
    obj["synopsis"] = synopsis;
    obj["synopsis_image"] =  QString( synopsis_image.toBase64() );
    obj["owned"] = owned;
    obj["number"] = number;
    obj["stars"] = stars;
    obj["comment"] = comment;
    obj["reprint"] = reprint;
    obj["read"] = read;


    QJsonArray indexArray;
    for (int i = 0; i < index.size(); ++i) {
        QJsonObject indexObject = index[i].toJson();
        indexArray.append(indexObject);
    }
    obj["index"] = indexArray;

    return obj;

}

QString Book::toString()
{
    QJsonObject obj = this->toJson();
    QJsonDocument doc(obj);
    return doc.toJson();

}

bool Book::isValid()
{
    return
       number != 0 &&
     ! title_ita.isEmpty() &&
     //! title_orig.isEmpty() && Can be original
     //! author.isEmpty() && Can be an antology
       date_pub.isValid() &&
     //! cover_author.isEmpty() &&  can be stock image
     ! cover_image.isEmpty() &&
     ( !synopsis.isEmpty() || !synopsis_image.isEmpty() ) ;

}


bool Book::fromHTML(const QString &path)
{


    std::ifstream inputFile(path.toStdString() , std::ios::in);
    if (!inputFile.is_open())
    {
      std::cerr << "Failed to open the HTML file." << std::endl;
      return false;
    }

    QFileInfo fileInfo(path);

    QString root = fileInfo.absolutePath();

    std::string htmlContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();



    TidyDoc tdoc = tidyCreate();
        TidyBuffer output = {0};
        TidyBuffer errbuf = {0};
        tidyBufInit(&output);
        tidyBufInit(&errbuf);

        // Configure Tidy for XHTML output

        tidyOptSetBool(tdoc, TidyXhtmlOut, yes);
        tidyOptSetBool(tdoc, TidyShowWarnings, no);
        tidyOptSetBool(tdoc, TidyDropEmptyElems, yes);
        tidyOptSetBool(tdoc, TidyDropPropAttrs, yes);

        tidyOptSetBool(tdoc, TidyQuiet, yes);
        tidyOptSetInt(tdoc, TidyWrapLen, 4096);

        // Parse the HTML content
        tidySetErrorBuffer(tdoc, &errbuf);
        if (tidyParseString(tdoc, htmlContent.c_str()) >= 0) {
            tidyCleanAndRepair(tdoc);
            tidyRunDiagnostics(tdoc);
            tidySaveBuffer(tdoc, &output);


            QString converted = QString( (char*)output.bp );
#ifdef VERBOSE
            qWarning().noquote() << "Converted XHTML:\n" << converted;
#endif

            converted = converted.replace( QRegularExpression("<p[^>]*>"), "" );
            converted = converted.replace( QRegularExpression("<font[^>]*>"), "" );
            converted = converted.replace( QRegularExpression("<div[^>]*>"), "");
            converted = converted.replace( "<i>", "", Qt::CaseInsensitive);
            converted = converted.replace( "<strong>", "", Qt::CaseInsensitive);
            converted = converted.replace( "<b>", "", Qt::CaseInsensitive);

            converted = converted.replace( "</b>", "", Qt::CaseInsensitive);
            converted = converted.replace( "</string>" , "", Qt::CaseInsensitive);
            converted = converted.replace( "</i>" , "", Qt::CaseInsensitive);
            converted = converted.replace( "</div>" , "", Qt::CaseInsensitive);
            converted = converted.replace( "</font>" , "", Qt::CaseInsensitive);
            converted = converted.replace( "</p>" , "", Qt::CaseInsensitive);

            converted = converted.replace( "Copertina di" , "", Qt::CaseInsensitive );



#ifdef VERBOSE
            qWarning().noquote() << "Converted XHTML:\n" << converted;
#endif

            QDomDocument doc;

            QDomDocument::ParseResult res = doc.setContent( converted );


            if (!res) {

                qDebug() << "Failed to parse the HTML content."<< res.errorMessage;

                return false;
            }

            int currentTable = 0;

            QDomNodeList tables = doc.elementsByTagName("table"); // Find the table
#ifdef VERBOSE
            qWarning() << "tables" << tables.size();
#endif

            if (tables.size() > 0 )
            {
                QDomNode table = tables.at(currentTable);

                QDomNodeList rows = table.toElement().elementsByTagName("tr"); // Find rows
#ifdef VERBOSE
                qWarning() <<"ROWS" << rows.size();
#endif
                int rowCount = 0;

                for (int i = 0; i < rows.size(); ++i)
                {
                    QDomNode row = rows.at(i);
                    QDomNodeList cells = row.toElement().elementsByTagName("td"); // Find cells
#ifdef VERBOSE
                    qWarning() <<"CELLS" << cells.size();
#endif
                  if ( cells.size() == 1 && i < 2   )
                    continue;

#ifdef VERBOSE
                   qWarning() <<"rowCount" << rowCount;
#endif
                    switch( rowCount++ )
                    {
                        case 0: // first row;
                        {
                            QString image_path = root + "/" + findImageSRC( cells.at(0) );

                            qWarning() << "Cover:" << image_path;

                            QFile img( image_path );

                            cover_image_path = image_path;

                            if ( img.open( QIODevice::ReadOnly) )
                            {
                                cover_image = img.readAll();
                                img.close();
                            }
                            else
                            {

                                qWarning() << "Image does not exist" << image_path;
                            }

                            QString value = findTextValue( cells.at(1) );
                            number = value.toInt();
                            qWarning() << "Number:" << value;

                            title_ita = findTextValue( cells.at(2) );
                            qWarning() << "Title:" << title_ita;



                        }
                        break;

                        case 1:
                        {
                            QString data = findTextValue( cells.at(0) );
                            date_pub = QDate::fromString( data, "d/M/yyyy" );
                            if ( !date_pub.isValid() )
                            {
                                date_pub = QDate::fromString( data, "M/yyyy" );
                            }

                            qWarning() << "Pubblicato il:" << date_pub;

                            title_orig = findTextValue( cells.at(1) );
                            qWarning() << "Title orig:" << title_orig;

                        }
                        break;

                        case 2:
                        {
                            cover_author = findTextValue( cells.at(0) );
                            if (cover_author.isEmpty() )
                            {
                                cover_author = "N/A";
                            }
                            qWarning() << "Cover author:" << cover_author;

                            author = findTextValue( cells.at(1) );
                            qWarning() << "Autore:" << author;

                        }
                        break;

                        case 3: break;

                        case 4:
                        {
                            QString text = findTextValue( cells.at(0) );
                            if ( text.isEmpty() )
                            {
                                QString image_path = root + "/" + findImageSRC( cells.at(0) );

                                qWarning() << "synopsis:" << image_path;

                                QFile img( image_path );
                                if ( img.open( QIODevice::ReadOnly) )
                                {
                                    synopsis_image = img.readAll();
                                    img.close();
                                }
                                else
                                {
                                    qWarning() << "synopsis Image does not exist" << image_path;
                                }
                            }
                            else
                            {
                                synopsis = text;
                                qWarning() << "Synopsis:" << synopsis;
                            }


                        }
                        break;

                        default:
                            break;
                    }



                }

                if (tables.size() > 1 ) //Indice
                {
                    QDomNode table = tables.at(1);

                    QDomNodeList rows = table.toElement().elementsByTagName("tr"); // Find rows

                    for (int i = 1; i < rows.size(); ++i) {

                        QDomNode row = rows.at(i);
                        QDomNodeList cells = row.toElement().elementsByTagName("td"); // Find cells
                        Index elem;

                        elem.title = findTextValue( cells.at(0) );
                        elem.author = findTextValue( cells.at(1) );

                        index.append( elem );

                    }
                }


            } else {
                qDebug() << "Failed to find the table.";
            }




        } else {
            qWarning() << "HTML Tidy encountered an error:\n" << QString( (char*) errbuf.bp );
        }

        // Clean up
        tidyBufFree(&output);
        tidyBufFree(&errbuf);
        tidyRelease(tdoc);


    return true;
}

auto operator<<(QDebug ds, const Book &book) -> QDebug
{
    ds.nospace() << const_cast<Book&>(  book ).toString();

    return ds.space();
}

auto operator<<(QTextStream &ds, const Book &book) -> QTextStream &
{
    ds << const_cast<Book&>(  book ).toString();
    return ds;
}

