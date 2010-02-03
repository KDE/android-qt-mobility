/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qversitresourcehandler.h"
#include "qversitproperty.h"
#include <QFile>

QTM_USE_NAMESPACE

/*!
 * \class QVersitResourceHandler
 * \preliminary
 * \brief The QVersitResourceHandler class is an interface for clients wishing to implement custom
 * behaviour for loading and saving files to disk when exporting and importing, respectively.
 *
 * \ingroup versit
 *
 * \sa QVersitContactImporter
 * \sa QVersitContactExporter
 *
 * \fn virtual bool QVersitResourceHandler::saveResource(const QByteArray& contents, const QVersitProperty& property, QString* location) = 0;
 * Saves the binary data \a contents to a file on a persistent storage medium.
 *
 * \a property holds the QVersitProperty which is the context in which the binary is coming from.
 * The QVersitResourceHandler can use this, for example, to determine file extension it should choose.
 * \a *filename is filled with the contents of the file.
 * Returns true on success, false on failure.
 *
 *
 * \fn virtual bool QVersitResourceHandler::loadResource(const QString& location, QByteArray* contents, QString* mimeType) = 0;
 * Loads a file from \a location.
 *
 * \a *contents is filled with the contents of the file and \a *mimeType is set to the MIME
 * type that it is determined to be.
 * Returns true on success, false on failure.
*/

/*!
 * Default resource loader.
 * Loads file from given \a location into \a contents and returns true if successful.
 * Does not set \a mimeType.
 */
bool QVersitDefaultResourceHandler::loadResource(const QString& location,
                                                 QByteArray* contents,
                                                 QString* mimeType)
{
    // XXX TODO: fill in the mimetype.
    Q_UNUSED(mimeType)
    if (location.isEmpty())
        return false;
    QFile file(location);
    file.open(QIODevice::ReadOnly);
    if (!file.isReadable())
        return false;
    *contents = file.readAll();
    return contents->size() > 0;
}

/*!
 * Default resource saver.
 * Does nothing and returns false.  By default, resources aren't persisted because we don't know
 * when it is safe to remove them.
 */
bool QVersitDefaultResourceHandler::saveResource(const QByteArray& contents,
                                                 const QVersitProperty& property,
                                                 QString* location)
{
    Q_UNUSED(contents)
    Q_UNUSED(property)
    Q_UNUSED(location)
    return false;
}
