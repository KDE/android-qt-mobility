/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qversitreader_p.h"
#include "versitutils.h"

// Some big enough value for nested versit documents to prevent infite recursion
#define MAX_VERSIT_DOCUMENT_NESTING_DEPTH 20

/*! Construct a reader. */
QVersitReaderPrivate::QVersitReaderPrivate() : 
    mIoDevice(0), 
    mDocumentNestingLevel(0)
{
}
    
/*! Destroy a reader. */    
QVersitReaderPrivate::~QVersitReaderPrivate()
{
}

/*!
 * Parses a versit document and returns the resulting document object
 */
QVersitDocument QVersitReaderPrivate::parseVersitDocument(QByteArray& text)
{
    mDocumentNestingLevel++;
    QVersitDocument document;
    text = text.mid(VersitUtils::countLeadingWhiteSpaces(text));
    QVersitProperty property = parseNextVersitProperty(text);
    if (property.name() == QString::fromAscii("BEGIN") && 
        property.value().trimmed().toUpper() == "VCARD") {
        while (property.name().length() > 0 && 
               property.name() != QString::fromAscii("END")) {
            property = parseNextVersitProperty(text);   
            if (!containsSupportedVersion(property)) {
                mDocumentNestingLevel--;
                return QVersitDocument(); // return an empty document
            }
            if (property.name() != QString::fromAscii("VERSION") && 
                property.name() != QString::fromAscii("END"))
                document.addProperty(property);
        }
    }
    mDocumentNestingLevel--;
    return document;
}

/*!
 * Parses a versit document and returns whether parsing succeeded.
 */
QVersitProperty QVersitReaderPrivate::parseNextVersitProperty(QByteArray& text)
{
    QVersitProperty property;
    property.setName(VersitUtils::extractPropertyName(text));
    property.setParameters(VersitUtils::extractPropertyParams(text));
    text = VersitUtils::extractPropertyValue(text); 
    if (property.name() == QString::fromAscii("AGENT")) {
        if (mDocumentNestingLevel >= MAX_VERSIT_DOCUMENT_NESTING_DEPTH)
            return property; // To prevent infinite recursion
        property.setEmbeddedDocument(parseVersitDocument(text));
    }
    else {
        int crlfPos = -1;
        QString encoding(QString::fromAscii("ENCODING"));
        QString quotedPrintable(QString::fromAscii("QUOTED-PRINTABLE"));
        if (property.parameters().contains(encoding,quotedPrintable)) {
            crlfPos = VersitUtils::findHardLineBreakInQuotedPrintable(text);
            QByteArray value = text.left(crlfPos);
            VersitUtils::decodeQuotedPrintable(value);
            // Remove the encoding parameter as the value is now decoded
            property.removeParameter(encoding,quotedPrintable);
            property.setValue(value);
        }
        else {
            crlfPos = text.indexOf("\r\n");
            QByteArray value = text.left(crlfPos);
            QString base64(QString::fromAscii("BASE64"));
            if (property.parameters().contains(encoding,base64)) {
                // Remove the linear whitespaces left by vCard 2.1 unfolding
                value.replace(' ',"");
                value.replace('\t',"");
            }
            property.setValue(value);
        }
        text = text.mid(crlfPos+2); // +2 is for skipping the CRLF
    }
    return property;
}

/*!
 * Checks whether the VERSION property contains a supported version.
 */
bool QVersitReaderPrivate::containsSupportedVersion(const QVersitProperty& property) const
{
    bool valid = true;
    if (property.name() == QString::fromAscii("VERSION")) {
        QByteArray value = property.value().trimmed();
        if (property.parameters().contains(
                QString::fromAscii("ENCODING"),QString::fromAscii("BASE64")))
            value = QByteArray::fromBase64(value);
        valid = (value == "2.1");
    } 
    return valid;
}
