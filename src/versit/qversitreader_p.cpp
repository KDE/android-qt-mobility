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
#include "qversitdocument.h"
#include "versitutils_p.h"
#include "qmobilityglobal.h"
#include <QTextCodec>
#include <QMutexLocker>
#include <QVariant>
#include <QBuffer>

QTM_USE_NAMESPACE

// Some big enough value for nested versit documents to prevent infinite recursion
#define MAX_VERSIT_DOCUMENT_NESTING_DEPTH 20

/*!
  \class LineReader
  \brief The LineReader class is a wrapper around a QIODevice that allows line-by-line reading.

  This class keeps an internal buffer which it uses to temporarily store data which it has read from
  the device but not returned to the user.
 */

/*!
  Constructs a LineReader that reads from the given \a device using the given \a codec.
  \a chunkSize is the number of bytes to read at a time (it is useful for testing but shouldn't
  otherwise be set).
  */
LineReader::LineReader(QIODevice* device, QTextCodec *codec, int chunkSize)
    : mDevice(device),
    mCodec(codec),
    mChunkSize(chunkSize),
    mCrlfList(*VersitUtils::newlineList(mCodec)),
    mBuffer(VersitCursor(QByteArray())),
    mOdometer(0)
{
}

/*!
  Attempts to read a line and returns a VersitCursor describing the line.  The cursor returned
  includes the data, as well as the position and selection index bounds.  Data within those bounds
  represents the line.  Data outside those bounds should not be used.
 */
VersitCursor LineReader::readLine()
{
    mBuffer.position = mBuffer.selection;
    mSearchFrom = mBuffer.position;

    // First, look for a newline in the already-existing buffer.  If found, return the line.
    if (tryReadLine(mBuffer, false)) {
        mBuffer.dropOldData();
        mOdometer += mBuffer.selection - mBuffer.position;
        return mBuffer;
    }

    // Otherwise, keep reading more data until either a CRLF is found, or there's no more to read.
    while (!mDevice->atEnd()) {
        QByteArray temp = mDevice->read(mChunkSize);
        mBuffer.data.append(temp);
        if (tryReadLine(mBuffer, false)) {
            mBuffer.dropOldData();
            mOdometer += mBuffer.selection - mBuffer.position;
            return mBuffer;
        }
    }

    // We've reached the end of the stream.  Find a newline from the buffer (or return what's left).
    tryReadLine(mBuffer, true);
    mBuffer.dropOldData();
    mOdometer += mBuffer.selection - mBuffer.position;
    return mBuffer;
}

/*!
  How many bytes have been returned in the VersitCursor in the lifetime of the LineReader.
 */
int LineReader::odometer()
{
    return mOdometer;
}

/*!
  Returns true if there are no more lines left for readLine() to return.  It is possible for atEnd()
  to return false and for there to be no more data left (eg. if there are trailing newlines at the
  end of the input.  In this case, readLine() will return an empty line (ie. position == selection).
 */
bool LineReader::atEnd()
{
    return mDevice->atEnd() && mBuffer.selection == mBuffer.data.size();
}

/*!
  Returns the codec that the LineReader reads with.
 */
QTextCodec* LineReader::codec()
{
    return mCodec;
}

/*!
 * Get the next line of input from \a device to parse.  Also performs unfolding by removing
 * sequences of newline-space from the retrieved line.  Skips over any newlines at the start of the
 * input.
 *
 * Returns a VersitCursor containing and selecting the line.
 */
bool LineReader::tryReadLine(VersitCursor &cursor, bool atEnd)
{
    int crlfPos;

    QByteArray space = VersitUtils::encode(' ', mCodec);
    QByteArray tab = VersitUtils::encode('\t', mCodec);
    int spaceLength = space.length();

    forever {
        foreach(const QByteArrayMatcher& crlf, mCrlfList) {
            int crlfLength = crlf.pattern().length();
            crlfPos = crlf.indexIn(cursor.data, mSearchFrom);
            if (crlfPos == cursor.position) {
                // Newline at start of line.  Set position to directly after it.
                cursor.position += crlfLength;
                mSearchFrom = cursor.position;
                break;
            } else if (crlfPos > cursor.position) {
                // Found the CRLF.
                if (QVersitReaderPrivate::containsAt(cursor.data, space, crlfPos + crlfLength)
                    || QVersitReaderPrivate::containsAt(cursor.data, tab, crlfPos + crlfLength)) {
                    // If it's followed by whitespace, collapse it.
                    cursor.data.remove(crlfPos, crlfLength + spaceLength);
                    mSearchFrom = crlfPos;
                    break;
                } else if (!atEnd && crlfPos + crlfLength + spaceLength >= cursor.data.size()) {
                    // If our CRLF is at the end of the current buffer but there's more to read,
                    // it's possible that a space could be hiding on the next read from the device.
                    // Just pretend we didn't see the CRLF and pick it up the next time round.
                    mSearchFrom = crlfPos;
                    return false;
                } else {
                    // Found the CRLF.
                    cursor.selection = crlfPos;
                    return true;
                }
            }
        }
        if (crlfPos == -1) {
            // No CRLF found.
            cursor.selection = cursor.data.size();
            return false;
        }
    }
}

/*! Construct a reader. */
QVersitReaderPrivate::QVersitReaderPrivate()
    : mIoDevice(0),
    mDocumentNestingLevel(0),
    mDefaultCodec(QTextCodec::codecForName("UTF-8")),
    mState(QVersitReader::InactiveState),
    mError(QVersitReader::NoError),
    mIsCanceling(false)
{
}

/*! Destroy a reader. */
QVersitReaderPrivate::~QVersitReaderPrivate()
{
}

/*!
 * Inherited from QThread, called by QThread when the thread has been started.
 */
void QVersitReaderPrivate::run()
{
    read();
}

/*!
 * Does the actual reading and sets the error and state as appropriate.
 * If \a async, then stateChanged() signals are emitted as the reading happens.
 */
void QVersitReaderPrivate::read()
{
    mMutex.lock();
    mVersitDocuments.clear();
    mMutex.unlock();
    bool canceled = false;

    LineReader lineReader(mIoDevice, mDefaultCodec);
    while(!lineReader.atEnd()) {
        if (isCanceling()) {
            canceled = true;
            break;
        }
        QVersitDocument document;
        int oldPos = lineReader.odometer();
        bool ok = parseVersitDocument(lineReader, document);

        if (ok) {
            if (document.isEmpty())
                break;
            else {
                QMutexLocker locker(&mMutex);
                mVersitDocuments.append(document);
                emit resultsAvailable(mVersitDocuments);
            }
        } else {
            setError(QVersitReader::ParseError);
            if (lineReader.odometer() == oldPos)
                break;
        }
    };
    if (canceled)
        setState(QVersitReader::CanceledState);
    else
        setState(QVersitReader::FinishedState);
}

void QVersitReaderPrivate::setState(QVersitReader::State state)
{
    mMutex.lock();
    mState = state;
    mMutex.unlock();
    emit stateChanged(state);
}

QVersitReader::State QVersitReaderPrivate::state() const
{
    QMutexLocker locker(&mMutex);
    return mState;
}

void QVersitReaderPrivate::setError(QVersitReader::Error error)
{
    QMutexLocker locker(&mMutex);
    mError = error;
}

QVersitReader::Error QVersitReaderPrivate::error() const
{
    QMutexLocker locker(&mMutex);
    return mError;
}

void QVersitReaderPrivate::setCanceling(bool canceling)
{
    QMutexLocker locker(&mMutex);
    mIsCanceling = canceling;
}

bool QVersitReaderPrivate::isCanceling()
{
    QMutexLocker locker(&mMutex);
    return mIsCanceling;
}

/*!
 * Parses a versit document. Returns true if the parsing was successful.
 */
bool QVersitReaderPrivate::parseVersitDocument(LineReader& lineReader, QVersitDocument& document,
                                               bool foundBegin)
{
    if (mDocumentNestingLevel >= MAX_VERSIT_DOCUMENT_NESTING_DEPTH)
        return false; // To prevent infinite recursion

    bool parsingOk = true;
    mDocumentNestingLevel++;

    // TODO: Various readers should be made subclasses and eliminate assumptions like this.
    // We don't know what type it is: just assume it's a vCard 3.0
    document.setType(QVersitDocument::VCard30Type);

    QVersitProperty property;

    if (!foundBegin) {
        property = parseNextVersitProperty(document.type(), lineReader);
        if (property.name() == QLatin1String("BEGIN")
            && property.value().trimmed().toUpper() == QLatin1String("VCARD")) {
            foundBegin = true;
        } else if (property.isEmpty()) {
            // A blank document (or end of file) was found.
            document = QVersitDocument();
        } else {
            // Some property other than BEGIN was found.
            parsingOk = false;
        }
    }

    if (foundBegin) {
        do {
            /* Grab it */
            property = parseNextVersitProperty(document.type(), lineReader);

            /* Discard embedded vcard documents - not supported yet.  Discard the entire vCard */
            if (property.name() == QLatin1String("BEGIN") &&
                QString::compare(property.value().trimmed(),
                                 QLatin1String("VCARD"), Qt::CaseInsensitive) == 0) {
                parsingOk = false;
                QVersitDocument nestedDocument;
                if (!parseVersitDocument(lineReader, nestedDocument, true))
                    break;
            }

            // See if this is a version property and continue parsing under that version
            if (!setVersionFromProperty(document, property)) {
                parsingOk = false;
                break;
            }

            /* Nope, something else.. just add it */
            if (property.name() != QLatin1String("VERSION") &&
                property.name() != QLatin1String("END"))
                document.addProperty(property);
        } while (property.name().length() > 0 && property.name() != QLatin1String("END"));
        if (property.name() != QLatin1String("END"))
            parsingOk = false;
    }
    mDocumentNestingLevel--;
    if (!parsingOk)
        document = QVersitDocument();

    return parsingOk;
}

/*!
 * Parses a versit document and returns whether parsing succeeded.
 */
QVersitProperty QVersitReaderPrivate::parseNextVersitProperty(
        QVersitDocument::VersitType versitType,
        LineReader& lineReader)
{
    VersitCursor cursor = lineReader.readLine();
    if (cursor.position >= cursor.selection)
        return QVersitProperty();

    // Otherwise, do stuff.
    QPair<QStringList,QString> groupsAndName =
            extractPropertyGroupsAndName(cursor, lineReader.codec());

    QVersitProperty property;
    property.setGroups(groupsAndName.first);
    property.setName(groupsAndName.second);

    if (versitType == QVersitDocument::VCard21Type)
        parseVCard21Property(cursor, property, lineReader);
    else if (versitType == QVersitDocument::VCard30Type)
        parseVCard30Property(cursor, property, lineReader);

    return property;
}

/*!
 * Parses the property according to vCard 2.1 syntax.
 */
void QVersitReaderPrivate::parseVCard21Property(VersitCursor& cursor, QVersitProperty& property,
                                                LineReader& lineReader)
{
    property.setParameters(extractVCard21PropertyParams(cursor, lineReader.codec()));

    QByteArray value = extractPropertyValue(cursor);
    if (property.name() == QLatin1String("AGENT")) {
        // Hack to handle cases where start of document is on the same or next line as "AGENT:"
        // XXX: Handle non-ASCII charsets in nested AGENT documents.
        bool foundBegin = false;
        if (value == "BEGIN:VCARD") {
            foundBegin = true;
        } else if (value.isEmpty()) {
        } else {
            property = QVersitProperty();
            return;
        }
        QVersitDocument agentDocument;
        if (!parseVersitDocument(lineReader, agentDocument, foundBegin)) {
            property = QVersitProperty();
        } else {
            property.setValue(QVariant::fromValue(agentDocument));
        }
    } else {
        QTextCodec* codec;
        QVariant valueVariant(decodeCharset(value, property, lineReader.codec(), &codec));
        unencode(valueVariant, cursor, property, codec, lineReader);
        property.setValue(valueVariant);
    }
}

/*!
 * Parses the property according to vCard 3.0 syntax.
 */
void QVersitReaderPrivate::parseVCard30Property(VersitCursor& cursor, QVersitProperty& property,
                                                LineReader& lineReader)
{
    property.setParameters(extractVCard30PropertyParams(cursor, lineReader.codec()));

    QByteArray value = extractPropertyValue(cursor);

    QTextCodec* codec;
    QString valueString(decodeCharset(value, property, lineReader.codec(), &codec));
    VersitUtils::removeBackSlashEscaping(valueString);

    if (property.name() == QLatin1String("AGENT")) {
        // Make a line reader from the value of the property.
        QByteArray agentValue(codec->fromUnicode(valueString));
        QBuffer agentData(&agentValue);
        agentData.open(QIODevice::ReadOnly);
        agentData.seek(0);
        LineReader agentLineReader(&agentData, codec);

        QVersitDocument agentDocument;
        if (!parseVersitDocument(agentLineReader, agentDocument)) {
            property = QVersitProperty();
        } else {
            property.setValue(QVariant::fromValue(agentDocument));
        }
    } else {
        QVariant valueVariant(valueString);
        unencode(valueVariant, cursor, property, codec, lineReader);
        if (valueVariant.type() == QVariant::ByteArray) {
            // hack: add the charset parameter back in (even if there wasn't one to start with and
            // the default codec was used).  This will help later on if someone calls valueString()
            // on the property.
            property.insertParameter(QLatin1String("CHARSET"), QLatin1String(codec->name()));
        }
        property.setValue(valueVariant);
    }
}

/*!
 * Sets version to \a document if \a property contains a supported version.
 */
bool QVersitReaderPrivate::setVersionFromProperty(QVersitDocument& document, const QVersitProperty& property) const
{
    bool valid = true;
    if (property.name() == QLatin1String("VERSION")) {
        QString value = property.value().trimmed();
        if (property.parameters().contains(QLatin1String("ENCODING"),QLatin1String("BASE64"))
            || property.parameters().contains(QLatin1String("TYPE"),QLatin1String("BASE64")))
            value = QLatin1String(QByteArray::fromBase64(value.toAscii()));
        if (value == QLatin1String("2.1")) {
            document.setType(QVersitDocument::VCard21Type);
        } else if (value == QLatin1String("3.0")) {
            document.setType(QVersitDocument::VCard30Type);
        } else {
            valid = false;
        }
    } 
    return valid;
}

/*!
 * On entry, \a value should hold a QString.  On exit, it may be either a QString or a QByteArray.
 */
void QVersitReaderPrivate::unencode(QVariant& value, VersitCursor& cursor,
                                    QVersitProperty& property, QTextCodec* codec,
                                    LineReader& lineReader) const
{
    Q_ASSERT(value.type() == QVariant::String);

    QString valueString = value.toString();

    if (property.parameters().contains(QLatin1String("ENCODING"), QLatin1String("QUOTED-PRINTABLE"))) {
        // At this point, we need to accumulate bytes until we hit a real line break (no = before
        // it) value already contains everything up to the character before the newline
        while (valueString.endsWith(QLatin1Char('='))) {
            valueString.chop(1); // Get rid of '='
            // We add each line (minus the escaped = and newline chars)
            cursor = lineReader.readLine();
            QString line = codec->toUnicode(
                    cursor.data.mid(cursor.position, cursor.selection-cursor.position));
            valueString.append(line);
        }
        decodeQuotedPrintable(valueString);
        // Remove the encoding parameter as the value is now decoded
        property.removeParameters(QLatin1String("ENCODING"));
        value.setValue(valueString);
    } else if (property.parameters().contains(QLatin1String("ENCODING"), QLatin1String("BASE64"))
        || property.parameters().contains(QLatin1String("ENCODING"), QLatin1String("B"))
        || property.parameters().contains(QLatin1String("TYPE"), QLatin1String("BASE64"))
        || property.parameters().contains(QLatin1String("TYPE"), QLatin1String("B"))) {
        value.setValue(QByteArray::fromBase64(valueString.toAscii()));
        // Remove the encoding parameter as the value is now decoded
        property.removeParameters(QLatin1String("ENCODING"));
        // Hack: add the charset parameter back in (even if there wasn't one to start with and
        // the default codec was used).  This will help later on if someone calls valueString()
        // on the property.
        property.insertParameter(QLatin1String("CHARSET"), QLatin1String(codec->name()));
    }
}

/*!
 * Decodes \a value, after working out what charset it is in using the context of \a property and
 * returns it.  The codec used to decode is returned in \a codec.
 */
QString QVersitReaderPrivate::decodeCharset(const QByteArray& value,
                                            QVersitProperty& property,
                                            QTextCodec* defaultCodec,
                                            QTextCodec** codec) const
{
    const QString charset(QLatin1String("CHARSET"));
    if (property.parameters().contains(charset)) {
        QString charsetValue = *property.parameters().find(charset);
        property.removeParameters(charset);
        *codec = QTextCodec::codecForName(charsetValue.toAscii());
        if (*codec != NULL) {
            return (*codec)->toUnicode(value);
        } else {
            *codec = defaultCodec;
            return defaultCodec->toUnicode(value);
        }
    }
    *codec = defaultCodec;
    return defaultCodec->toUnicode(value);
}

/*!
 * Decodes Quoted-Printable encoded (RFC 1521) characters in /a text.
 */
void QVersitReaderPrivate::decodeQuotedPrintable(QString& text) const
{
    for (int i=0; i < text.length(); i++) {
        QChar current = text.at(i);
        if (current == QLatin1Char('=') && i+2 < text.length()) {
            int next = text.at(i+1).unicode();
            int nextAfterNext = text.at(i+2).unicode();
            if (((next >= 'a' && next <= 'f') ||
                 (next >= 'A' && next <= 'F') ||
                 (next >= '0' && next <= '9')) &&
                ((nextAfterNext >= 'a' && nextAfterNext <= 'f') ||
                 (nextAfterNext >= 'A' && nextAfterNext <= 'F') ||
                 (nextAfterNext >= '0' && nextAfterNext <= '9'))) {
                bool ok;
                QChar decodedChar(text.mid(i+1, 2).toInt(&ok,16));
                if (ok)
                    text.replace(i, 3, decodedChar);
            } else if (next == '\r' && nextAfterNext == '\n') {
                // Newlines can still be found here if they are encoded in a non-default charset.
                text.remove(i, 3);
            }
        }
    }
}


/*!
 * Extracts the groups and the name of the property using \a codec to determine the delimiters
 *
 * On entry, \a line should select a whole line.
 * On exit, \a line will be updated to point after the groups and name.
 */
QPair<QStringList,QString>QVersitReaderPrivate::extractPropertyGroupsAndName(
        VersitCursor& line, QTextCodec *codec) const
{
    const QByteArray semicolon = VersitUtils::encode(';', codec);
    const QByteArray colon = VersitUtils::encode(':', codec);
    const QByteArray backslash = VersitUtils::encode('\\', codec);
    QPair<QStringList,QString> groupsAndName;
    int length = 0;
    Q_ASSERT(line.data.size() >= line.position);

    int separatorLength = semicolon.length();
    for (int i = line.position; i < line.selection - separatorLength + 1; i++) {
        if ((containsAt(line.data, semicolon, i)
                && !containsAt(line.data, backslash, i-separatorLength))
            || containsAt(line.data, colon, i)) {
            length = i - line.position;
            break;
        }
    }
    if (length > 0) {
        QString trimmedGroupsAndName =
                codec->toUnicode(line.data.mid(line.position, length)).trimmed();
        QStringList parts = trimmedGroupsAndName.split(QLatin1Char('.'));
        if (parts.count() > 1) {
            groupsAndName.second = parts.takeLast();
            groupsAndName.first = parts;
        } else {
            groupsAndName.second = trimmedGroupsAndName;
        }
        line.setPosition(length + line.position);
    }

    return groupsAndName;
}

/*!
 * Extracts the value of the property.
 * Returns an empty string if the value was not found.
 *
 * On entry \a line should point to the value anyway.
 * On exit \a line should point to newline after the value
 */
QByteArray QVersitReaderPrivate::extractPropertyValue(VersitCursor& line) const
{
    QByteArray value = line.data.mid(line.position, line.selection - line.position);

    /* Now advance the cursor in all cases. */
    line.position = line.selection;
    return value;
}

/*!
 * Extracts the property parameters as a QMultiHash using \a codec to determine the delimiters.
 * The parameters without names are added as "TYPE" parameters.
 *
 * On entry \a line should contain the entire line.
 * On exit, line will be updated to point to the start of the value.
 */
QMultiHash<QString,QString> QVersitReaderPrivate::extractVCard21PropertyParams(
        VersitCursor& line, QTextCodec *codec) const
{
    QMultiHash<QString,QString> result;
    QList<QByteArray> paramList = extractParams(line, codec);
    while (!paramList.isEmpty()) {
        QByteArray param = paramList.takeLast();
        QString name = paramName(param, codec);
        QString value = paramValue(param, codec);
        result.insert(name,value);
    }

    return result;
}

/*!
 * Extracts the property parameters as a QMultiHash using \a codec to determine the delimiters.
 * The parameters without names are added as "TYPE" parameters.
 */
QMultiHash<QString,QString> QVersitReaderPrivate::extractVCard30PropertyParams(
        VersitCursor& line, QTextCodec *codec) const
{
    QMultiHash<QString,QString> result;
    QList<QByteArray> paramList = extractParams(line, codec);
    while (!paramList.isEmpty()) {
        QByteArray param = paramList.takeLast();
        QString name(paramName(param, codec));
        VersitUtils::removeBackSlashEscaping(name);
        QString values = paramValue(param, codec);
        QList<QString> valueList = values.split(QLatin1Char(','), QString::SkipEmptyParts);
        QString buffer; // for any part ending in a backslash, join it to the next.
        foreach (QString value, valueList) {
            if (value.endsWith(QLatin1Char('\\')) && !value.endsWith(QLatin1String("\\\\"))) {
                value.chop(1);
                buffer.append(value);
                buffer.append(QLatin1Char(',')); // because the comma got nuked by split()
            }
            else {
                buffer.append(value);
                VersitUtils::removeBackSlashEscaping(buffer);
                result.insert(name, buffer);
                buffer.clear();
            }
        }
    }

    return result;
}


/*!
 * Extracts the parameters as delimited by semicolons using \a codec to determine the delimiters.
 *
 * On entry \a line should point to the start of the parameter section (past the name).
 * On exit, \a line will be updated to point to the start of the value.
 */
QList<QByteArray> QVersitReaderPrivate::extractParams(VersitCursor& line, QTextCodec *codec) const
{
    const QByteArray colon = VersitUtils::encode(':', codec);
    QList<QByteArray> params;

    /* find the end of the name&params */
    int colonIndex = line.data.indexOf(colon, line.position);
    if (colonIndex > line.position && colonIndex < line.selection) {
        QByteArray nameAndParamsString = line.data.mid(line.position, colonIndex - line.position);
        params = extractParts(nameAndParamsString, VersitUtils::encode(';', codec), codec);

        /* Update line */
        line.setPosition(colonIndex + colon.length());
    } else if (colonIndex == line.position) {
        // No parameters.. advance past it
        line.setPosition(line.position + colon.length());
    }

    return params;
}

/*!
 * Extracts the parts separated by separator discarding the separators escaped with a backslash
 * encoded with \a codec
 */
QList<QByteArray> QVersitReaderPrivate::extractParts(
        const QByteArray& text, const QByteArray& separator, QTextCodec* codec) const
{
    QList<QByteArray> parts;
    int partStartIndex = 0;
    int textLength = text.length();
    int separatorLength = separator.length();
    QByteArray backslash = VersitUtils::encode('\\', codec);
    int backslashLength = backslash.length();

    for (int i=0; i < textLength-separatorLength+1; i++) {
        if (containsAt(text, separator, i)
            && (i < backslashLength
                || !containsAt(text, backslash, i-backslashLength))) {
            int length = i-partStartIndex;
            QByteArray part = extractPart(text,partStartIndex,length);
            if (part.length() > 0)
                parts.append(part);
            partStartIndex = i+separatorLength;
        }
    }

    // Add the last or only part
    QByteArray part = extractPart(text,partStartIndex);
    if (part.length() > 0)
        parts.append(part);
    return parts;
}

/*!
 * Extracts a substring limited by /a startPosition and /a length.
 */
QByteArray QVersitReaderPrivate::extractPart(
        const QByteArray& text, int startPosition, int length) const
{
    QByteArray part;
    if (startPosition >= 0)
        part = text.mid(startPosition,length).trimmed();
    return part;
}

/*!
 * Extracts the name of the parameter using \a codec to determine the delimiters.
 * No name is interpreted as an implicit "TYPE".
 */
QString QVersitReaderPrivate::paramName(const QByteArray& parameter, QTextCodec* codec) const
{
     if (parameter.trimmed().length() == 0)
         return QString();
     QByteArray equals = VersitUtils::encode('=', codec);
     int equalsIndex = parameter.indexOf(equals);
     if (equalsIndex > 0) {
         return codec->toUnicode(parameter.left(equalsIndex)).trimmed();
     }

     return QLatin1String("TYPE");
}

/*!
 * Extracts the value of the parameter using \a codec to determine the delimiters
 */
QString QVersitReaderPrivate::paramValue(const QByteArray& parameter, QTextCodec* codec) const
{
    QByteArray value(parameter);
    QByteArray equals = VersitUtils::encode('=', codec);
    int equalsIndex = parameter.indexOf(equals);
    if (equalsIndex > 0) {
        int valueLength = parameter.length() - (equalsIndex + equals.length());
        value = parameter.right(valueLength).trimmed();
    }

    return codec->toUnicode(value);
}

/*!
 * Returns true if and only if \a text contains \a ba at \a index
 *
 * On entry, index must be >= 0
 */
bool QVersitReaderPrivate::containsAt(const QByteArray& text, const QByteArray& match, int index)
{
    int n = match.length();
    if (text.length() - index < n)
        return false;
    const char* textData = text.constData();
    const char* matchData = match.constData();
    return memcmp(textData+index, matchData, n) == 0;
}

#include "moc_qversitreader_p.cpp"
