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


#include "qversitcontactexporter.h"
#include "qversitcontactexporter_p.h"
#include "qmobilityglobal.h"

#include <qcontact.h>
#include <qcontactdetail.h>

QTM_USE_NAMESPACE

/*!
 * \class QVersitContactExporterDetailHandler
 * \preliminary
 * \brief The QVersitContactExporterDetailHandler class is an interface for clients wishing to implement
 * custom export behaviour for certain contact details.
 *
 * \ingroup versit
 *
 * \sa QVersitContactExporter
 */

/*!
 * \fn virtual bool QVersitContactExporterDetailHandler::preProcessDetail(const QContact& contact, const QContactDetail& detail, QVersitDocument* document) = 0;
 * Process \a detail and update \a document with the corresponding QVersitProperty(s).
 * \a contact provides the context within which the detail was found.
 *
 * Returns true if the detail has been handled and requires no furthur processing, false otherwise.
 *
 * This function is called on every QContactDetail encountered during an export.  Supply this
 * function and return true to implement custom export behaviour.
 */

/*!
 * \fn virtual bool QVersitContactExporterDetailHandler::postProcessDetail(const QContact& contact, const QContactDetail& detail, bool alreadyProcessed, QVersitDocument* document) = 0;
 * Process \a detail and update \a document with the corresponding QVersitProperty(s).
 * \a contact provides the context within which the detail was found.
 *
 * Returns true if the detail has been handled, false otherwise.
 *
 * This function is called on every \l QContactDetail encountered during an export which is not
 * handled by either \l preProcessDetail() or by QVersitContactExporter.  This can be used to
 * implement support for \l QContactDetail(s) not supported by QVersitContactExporter.
 */

/*!
 * \class QVersitContactExporter
 * \preliminary
 * \brief The QVersitContactExporter class converts \l QContact(s) into \l QVersitDocument(s).
 *
 * \ingroup versit
 *
 * A \l QVersitResourceHandler is associated with the exporter to supply the behaviour for loading
 * files from persistent storage.  By default, this is set to a \l QVersitDefaultResourceHandler,
 * which supports basic resource loading from the file system.  An alternative resource handler
 * can be specified with setResourceHandler().
 *
 * By associating a \l QVersitContactExporterDetailHandler with the exporter using
 * setDetailHandler(), the client can pass in a handler to override the processing of details and/or
 * handle details that QVersitContactExporter doesn't support.
 *
 * \code
 *
 * class MyDetailHandler : public QVersitContactExporterDetailHandler {
 * public:
 *     bool preProcessDetail(const QContact& contact, const QContactDetail& detail, QVersitDocument* document) {
 *         return false;
 *     }
 *     bool postProcessDetail(const QContact& contact, const QContactDetail& detail, bool alreadyProcessed, QVersitDocument* document) {
 *         if (!alreadyProcessed)
 *             mUnknownDetails.append(detail);
 *         return false;
 *     }
 *     QList<QContactDetail> mUnknownDetails;
 * };
 *
 * // An example of exporting a QContact:
 * QVersitContactExporter contactExporter;
 *
 * MyDetailHandler detailHandler;
 * contactExporter.setDetailHandler(&detailHandler);
 *
 * QContact contact;
 * // Create a name
 * QContactName name;
 * name.setFirst(QString::fromAscii("John"));
 * contact.saveDetail(&name);
 *
 * // Create an avatar type which is not supported by the exporter
 * QContactAvatar contactAvatar;
 * contactAvatar.setAvatar(QString::fromAscii("/my/image/avatar_path/texture.type"));
 * contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
 * contact.saveDetail(&contactAvatar);
 *
 * // Create an organization detail with a title and a logo
 * QContactOrganization organization;
 * organization.setTitle(QString::fromAscii("Developer"));
 * organization.setLogo(QString::fromAscii("/my/image/logo_path/logo.jpg"));
 * contact.saveDetail(&organization);
 *
 * QList<QContact> contactList;
 * contactList.append(contact);
 * QList<QVersitDocument> versitDocuments = contactExporter.exportContacts(contactList);
 *
 * // detailHandler.mUnknownDetails now contains the list of unknown details
 *
 * \endcode
 *
 * \sa QVersitDocument, QVersitProperty, QVersitContactExporterDetailHandler, QVersitResourceLoader
 */

/*!
 * Constructs a new contact exporter
 */
QVersitContactExporter::QVersitContactExporter()
    : d(new QVersitContactExporterPrivate())
{
}

/*!
 * Frees any memory in use by this contact exporter.
 */
QVersitContactExporter::~QVersitContactExporter()
{
    delete d;
}

/*!
 * Converts \a contacts into a list of corresponding QVersitDocuments, using the format given by
 * \a versitType.
 */
QList<QVersitDocument> QVersitContactExporter::exportContacts(
    const QList<QContact>& contacts,
    QVersitDocument::VersitType versitType)
{
    QList<QVersitDocument> list;
    foreach (QContact contact, contacts) {
        QVersitDocument versitDocument;
        versitDocument.setType(versitType);
        d->exportContact(contact, versitDocument);
        list.append(versitDocument);
    }

    return list;
}

/*!
 * Sets \a handler to be the handler for processing QContactDetails.
 */
void QVersitContactExporter::setDetailHandler(QVersitContactExporterDetailHandler* handler)
{
    d->mDetailHandler = handler;
}

/*!
 * Gets the handler for processing QContactDetails.
 */
QVersitContactExporterDetailHandler* QVersitContactExporter::detailHandler() const
{
    return d->mDetailHandler;
}

/*!
 * Sets \a handler to be the handler to load files with.
 */
void QVersitContactExporter::setResourceHandler(QVersitResourceHandler* handler)
{
    d->mResourceHandler = handler;
}

/*!
 * Returns the associated resource handler.
 */
QVersitResourceHandler* QVersitContactExporter::resourceHandler() const
{
    return d->mResourceHandler;
}
